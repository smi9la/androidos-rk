# API Server Implementation for R36S
# This runs on the R36S device and provides REST API + WebSocket interface

from flask import Flask, request, jsonify, send_file
from flask_cors import CORS
from flask_socketio import SocketIO, emit, disconnect
import json
import subprocess
import threading
import time
import os
import psutil
import sqlite3
from datetime import datetime
import logging

app = Flask(__name__)
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*")

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# ============== SYSTEM ENDPOINTS ==============

@app.route('/api/v1/system/info', methods=['GET'])
def system_info():
    """Get device information"""
    try:
        return jsonify({
            'device_name': 'R36S',
            'model': 'R36S RK3326',
            'os_version': '1.0.0',
            'kernel_version': '5.10.197',
            'cpu_model': 'ARM Cortex-A35',
            'cpu_cores': 4,
            'ram_total': 1024,  # MB
            'storage_total': 32000,  # MB
        })
    except Exception as e:
        logger.error(f"Error getting system info: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/system/stats', methods=['GET'])
def system_stats():
    """Get real-time system statistics"""
    try:
        # CPU usage
        cpu_usage = psutil.cpu_percent(interval=1)
        
        # Memory
        mem = psutil.virtual_memory()
        ram_usage = mem.used // (1024 * 1024)  # Convert to MB
        
        # Temperature (read from sysfs)
        temp = read_temperature()
        
        # Battery
        battery_info = read_battery_info()
        
        # Get FPS from running emulator (placeholder)
        fps = 60
        
        return jsonify({
            'cpu_usage': cpu_usage,
            'ram_usage': ram_usage,
            'temperature': temp,
            'battery': battery_info['level'],
            'battery_status': battery_info['status'],
            'storage_used': get_storage_used(),
            'storage_total': 32000,
            'fps': fps,
            'timestamp': int(time.time() * 1000)
        })
    except Exception as e:
        logger.error(f"Error getting system stats: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/system/time', methods=['GET'])
def system_time():
    """Get current system time"""
    return jsonify({
        'timestamp': int(time.time()),
        'iso': datetime.now().isoformat()
    })

@app.route('/api/v1/system/shutdown', methods=['POST'])
def shutdown():
    """Shutdown device"""
    try:
        subprocess.run(['systemctl', 'poweroff'], check=True)
        return jsonify({'status': 'shutdown initiated'})
    except Exception as e:
        logger.error(f"Shutdown error: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/system/reboot', methods=['POST'])
def reboot():
    """Reboot device"""
    try:
        subprocess.run(['systemctl', 'reboot'], check=True)
        return jsonify({'status': 'reboot initiated'})
    except Exception as e:
        logger.error(f"Reboot error: {e}")
        return jsonify({'error': str(e)}), 500

# ============== GAMES & ROMS ==============

@app.route('/api/v1/games/list', methods=['GET'])
def games_list():
    """Get list of installed games"""
    try:
        conn = sqlite3.connect('/var/lib/androidos/games.db')
        cursor = conn.cursor()
        cursor.execute('SELECT id, title, emulator, rom_path, cover_image FROM games ORDER BY title')
        games = [{
            'id': row[0],
            'title': row[1],
            'emulator': row[2],
            'rom_path': row[3],
            'cover': row[4]
        } for row in cursor.fetchall()]
        conn.close()
        return jsonify(games)
    except Exception as e:
        logger.error(f"Error getting games list: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/games/<game_id>', methods=['GET'])
def game_details(game_id):
    """Get game details"""
    try:
        conn = sqlite3.connect('/var/lib/androidos/games.db')
        cursor = conn.cursor()
        cursor.execute('SELECT * FROM games WHERE id = ?', (game_id,))
        game = cursor.fetchone()
        conn.close()
        
        if not game:
            return jsonify({'error': 'Game not found'}), 404
        
        return jsonify({
            'id': game[0],
            'title': game[1],
            'emulator': game[2],
            'rom_path': game[3],
            'cover': game[4],
            'description': game[5],
            'rating': game[6],
            'playtime': game[7]
        })
    except Exception as e:
        logger.error(f"Error getting game details: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/games/launch/<game_id>', methods=['POST'])
def launch_game(game_id):
    """Launch a game"""
    try:
        conn = sqlite3.connect('/var/lib/androidos/games.db')
        cursor = conn.cursor()
        cursor.execute('SELECT emulator, rom_path FROM games WHERE id = ?', (game_id,))
        result = cursor.fetchone()
        conn.close()
        
        if not result:
            return jsonify({'error': 'Game not found'}), 404
        
        emulator, rom_path = result
        
        # Launch emulator with ROM
        if emulator == 'ppsspp':
            proc = subprocess.Popen(['/usr/bin/ppsspp', rom_path])
        elif emulator == 'retroarch':
            proc = subprocess.Popen(['/usr/bin/retroarch', rom_path])
        else:
            return jsonify({'error': 'Unknown emulator'}), 400
        
        logger.info(f"Launched {game_id} with PID {proc.pid}")
        
        return jsonify({
            'status': 'launched',
            'pid': proc.pid,
            'game_id': game_id
        })
    except Exception as e:
        logger.error(f"Error launching game: {e}")
        return jsonify({'error': str(e)}), 500

# ============== INPUT HANDLING ==============

@app.route('/api/v1/input/event', methods=['POST'])
def input_event():
    """Send input event to active application"""
    try:
        data = request.get_json()
        button = data.get('button')
        state = data.get('state')
        
        # Send to active emulator/app via named pipe or socket
        # Implementation depends on emulator's input interface
        
        logger.info(f"Input event: {button} = {state}")
        
        # Broadcast to connected WebSocket clients
        socketio.emit('input_event', {
            'button': button,
            'state': state,
            'timestamp': int(time.time() * 1000)
        }, broadcast=True)
        
        return jsonify({'status': 'ok'})
    except Exception as e:
        logger.error(f"Error handling input: {e}")
        return jsonify({'error': str(e)}), 500

# ============== STORAGE ==============

@app.route('/api/v1/storage/info', methods=['GET'])
def storage_info():
    """Get storage information"""
    try:
        disk = psutil.disk_usage('/')
        return jsonify({
            'total': disk.total // (1024 * 1024),  # MB
            'used': disk.used // (1024 * 1024),
            'free': disk.free // (1024 * 1024),
            'percent': disk.percent
        })
    except Exception as e:
        logger.error(f"Error getting storage info: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/files/browse/<path:filepath>', methods=['GET'])
def browse_files(filepath):
    """Browse file system"""
    try:
        full_path = os.path.join('/roms', filepath)
        if not os.path.exists(full_path):
            return jsonify({'error': 'Path not found'}), 404
        
        files = []
        if os.path.isdir(full_path):
            for item in os.listdir(full_path):
                item_path = os.path.join(full_path, item)
                files.append({
                    'name': item,
                    'type': 'dir' if os.path.isdir(item_path) else 'file',
                    'size': os.path.getsize(item_path) if os.path.isfile(item_path) else 0
                })
        
        return jsonify(files)
    except Exception as e:
        logger.error(f"Error browsing files: {e}")
        return jsonify({'error': str(e)}), 500

# ============== POWER ==============

@app.route('/api/v1/power/battery', methods=['GET'])
def battery_info():
    """Get battery information"""
    try:
        info = read_battery_info()
        return jsonify(info)
    except Exception as e:
        logger.error(f"Error getting battery info: {e}")
        return jsonify({'error': str(e)}), 500

@app.route('/api/v1/power/thermal', methods=['GET'])
def thermal_info():
    """Get thermal information"""
    try:
        temp = read_temperature()
        return jsonify({
            'zones': [{
                'name': 'CPU',
                'temp': temp
            }],
            'warning_threshold': 75,
            'critical_threshold': 85
        })
    except Exception as e:
        logger.error(f"Error getting thermal info: {e}")
        return jsonify({'error': str(e)}), 500

# ============== HELPER FUNCTIONS ==============

def read_battery_info():
    """Read battery info from sysfs"""
    try:
        with open('/sys/class/power_supply/battery/capacity', 'r') as f:
            capacity = int(f.read().strip())
        
        with open('/sys/class/power_supply/battery/status', 'r') as f:
            status = f.read().strip()
        
        with open('/sys/class/power_supply/battery/temp', 'r') as f:
            temp = int(f.read().strip()) / 10.0
        
        with open('/sys/class/power_supply/battery/voltage_now', 'r') as f:
            voltage = int(f.read().strip()) / 1000000  # Convert to volts
        
        return {
            'level': capacity,
            'status': status,
            'temperature': temp,
            'voltage': voltage,
            'health': 'Good'
        }
    except Exception as e:
        logger.error(f"Error reading battery: {e}")
        return {'level': 0, 'status': 'Unknown'}

def read_temperature():
    """Read CPU temperature from sysfs"""
    try:
        with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
            temp = int(f.read().strip()) / 1000.0  # millidegrees to degrees
        return temp
    except Exception:
        return 0.0

def get_storage_used():
    """Get used storage in MB"""
    try:
        disk = psutil.disk_usage('/')
        return disk.used // (1024 * 1024)
    except Exception:
        return 0

# ============== WEBSOCKET ==============

@socketio.on('connect')
def handle_connect():
    logger.info(f"Client connected: {request.sid}")
    emit('connected', {'data': 'Connected to R36S'})

@socketio.on('disconnect')
def handle_disconnect():
    logger.info(f"Client disconnected: {request.sid}")

@socketio.on('subscribe_stats')
def handle_subscribe_stats():
    """Subscribe to real-time stats updates"""
    def emit_stats():
        while True:
            try:
                stats = {
                    'event': 'system_stats',
                    'data': {
                        'cpu_usage': psutil.cpu_percent(),
                        'ram_usage': psutil.virtual_memory().used // (1024 * 1024),
                        'temperature': read_temperature(),
                        'battery': read_battery_info()['level'],
                        'fps': 60,
                        'timestamp': int(time.time() * 1000)
                    }
                }
                socketio.emit('stats_update', stats, room=request.sid)
                time.sleep(1)
            except Exception as e:
                logger.error(f"Error emitting stats: {e}")
                break
    
    thread = threading.Thread(target=emit_stats, daemon=True)
    thread.start()

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=8888, debug=False)
