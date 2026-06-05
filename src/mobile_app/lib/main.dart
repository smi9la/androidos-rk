import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:async';

void main() {
  runApp(const AndroidOSApp());
}

class AndroidOSApp extends StatelessWidget {
  const AndroidOSApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'AndriodOS-RK',
      theme: ThemeData(
        brightness: Brightness.dark,
        primaryColor: Color(0xFF00B4DC),
        scaffoldBackgroundColor: Color(0xFF0F0F0F),
        useMaterial3: true,
      ),
      home: const HomeScreen(),
    );
  }
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  String? connectedDeviceIp;
  Map<String, dynamic>? deviceInfo;
  Map<String, dynamic>? deviceStats;
  final FlutterBlue flutterBlue = FlutterBlue.instance;
  Timer? statsTimer;

  @override
  void initState() {
    super.initState();
    _discoverDevices();
  }

  @override
  void dispose() {
    statsTimer?.cancel();
    super.dispose();
  }

  Future<void> _discoverDevices() async {
    // Scan network for R36S devices
    final devices = <Map<String, dynamic>>[];
    
    for (int i = 1; i < 255; i++) {
      final ip = '192.168.1.$i';
      try {
        final response = await http
            .get(
              Uri.parse('http://$ip:8888/api/v1/system/info'),
              headers: {'Connection': 'close'},
            )
            .timeout(
              const Duration(milliseconds: 500),
            );
        
        if (response.statusCode == 200) {
          final data = jsonDecode(response.body);
          devices.add({
            'ip': ip,
            'name': data['device_name'] ?? 'R36S Device',
            'model': data['model'] ?? 'Unknown',
            'online': true,
          });
        }
      } catch (e) {
        // Device not found
      }
    }

    if (mounted) {
      setState(() {});
    }
  }

  Future<void> _connectToDevice(String ip) async {
    try {
      final response = await http.get(
        Uri.parse('http://$ip:8888/api/v1/system/info'),
      );

      if (response.statusCode == 200) {
        final info = jsonDecode(response.body);
        setState(() {
          connectedDeviceIp = ip;
          deviceInfo = info;
        });
        _startStatsPolling();
      }
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Failed to connect: $e')),
      );
    }
  }

  void _startStatsPolling() {
    statsTimer?.cancel();
    statsTimer = Timer.periodic(Duration(seconds: 1), (_) async {
      if (connectedDeviceIp != null) {
        try {
          final response = await http.get(
            Uri.parse('http://$connectedDeviceIp:8888/api/v1/system/stats'),
          );

          if (response.statusCode == 200) {
            final stats = jsonDecode(response.body);
            setState(() {
              deviceStats = stats;
            });
          }
        } catch (e) {
          // Error fetching stats
        }
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      length: 3,
      child: Scaffold(
        appBar: AppBar(
          title: const Text('AndriodOS-RK'),
          elevation: 0,
          bottom: const TabBar(
            tabs: [
              Tab(icon: Icon(Icons.home), text: 'Home'),
              Tab(icon: Icon(Icons.gamepad), text: 'Controller'),
              Tab(icon: Icon(Icons.settings), text: 'Settings'),
            ],
          ),
        ),
        body: TabBarView(
          children: [
            _buildHomeTab(),
            _buildControllerTab(),
            _buildSettingsTab(),
          ],
        ),
      ),
    );
  }

  Widget _buildHomeTab() {
    if (connectedDeviceIp == null) {
      return _buildDeviceDiscovery();
    }

    return SingleChildScrollView(
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Device Status Card
            Card(
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(12),
              ),
              color: Color(0xFF1A1A1A),
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Text(
                              deviceInfo?['device_name'] ?? 'R36S',
                              style: Theme.of(context).textTheme.headlineSmall,
                            ),
                            Text(
                              '🟢 Online',
                              style: TextStyle(color: Colors.green),
                            ),
                          ],
                        ),
                        ElevatedButton.icon(
                          onPressed: () => _connectToDevice(''),
                          icon: Icon(Icons.close),
                          label: Text('Disconnect'),
                        ),
                      ],
                    ),
                    SizedBox(height: 16),
                    Text(
                      'Battery: ${deviceStats?['battery'] ?? 0}%',
                      style: Theme.of(context).textTheme.bodyMedium,
                    ),
                    ClipRRect(
                      borderRadius: BorderRadius.circular(8),
                      child: LinearProgressIndicator(
                        value: (deviceStats?['battery'] ?? 0) / 100,
                        minHeight: 8,
                      ),
                    ),
                    SizedBox(height: 12),
                    Text(
                      'Storage: ${deviceStats?['storage_used'] ?? 0}GB / ${deviceStats?['storage_total'] ?? 0}GB',
                      style: Theme.of(context).textTheme.bodySmall,
                    ),
                  ],
                ),
              ),
            ),
            SizedBox(height: 24),

            // Quick Actions
            Text(
              'Quick Actions',
              style: Theme.of(context).textTheme.titleMedium,
            ),
            SizedBox(height: 12),
            GridView.count(
              crossAxisCount: 2,
              shrinkWrap: true,
              physics: NeverScrollableScrollPhysics(),
              mainAxisSpacing: 12,
              crossAxisSpacing: 12,
              children: [
                _buildQuickActionCard('Start Game', Icons.play_arrow, () {}),
                _buildQuickActionCard('Monitor', Icons.analytics, () {}),
                _buildQuickActionCard('Sync', Icons.sync, () {}),
                _buildQuickActionCard('Backup', Icons.backup, () {}),
              ],
            ),
            SizedBox(height: 24),

            // System Stats
            Text(
              'System Status',
              style: Theme.of(context).textTheme.titleMedium,
            ),
            SizedBox(height: 12),
            _buildStatRow(
              'CPU Usage',
              '${deviceStats?['cpu_usage'] ?? 0}%',
              (deviceStats?['cpu_usage'] ?? 0) / 100,
            ),
            _buildStatRow(
              'RAM Usage',
              '${deviceStats?['ram_usage'] ?? 0}MB',
              (deviceStats?['ram_usage'] ?? 0) / 1024,
            ),
            _buildStatRow(
              'Temperature',
              '${deviceStats?['temperature'] ?? 0}°C',
              null,
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildDeviceDiscovery() {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Icon(
            Icons.devices,
            size: 64,
            color: Colors.grey,
          ),
          SizedBox(height: 16),
          Text(
            'No Device Connected',
            style: Theme.of(context).textTheme.headlineSmall,
          ),
          SizedBox(height: 8),
          Text(
            'Scan your network for R36S devices',
            style: Theme.of(context).textTheme.bodySmall,
          ),
          SizedBox(height: 24),
          ElevatedButton.icon(
            onPressed: _discoverDevices,
            icon: Icon(Icons.search),
            label: Text('Discover Devices'),
          ),
        ],
      ),
    );
  }

  Widget _buildControllerTab() {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          if (connectedDeviceIp != null)
            Expanded(
              child: SingleChildScrollView(
                child: Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Column(
                    children: [
                      // D-Pad
                      SizedBox(
                        width: 140,
                        height: 140,
                        child: Stack(
                          children: [
                            Center(
                              child: Container(
                                width: 100,
                                height: 100,
                                decoration: BoxDecoration(
                                  color: Color(0xFF2D2D2D),
                                  borderRadius: BorderRadius.circular(12),
                                ),
                              ),
                            ),
                            // D-Pad buttons
                            Positioned(
                              top: 0,
                              left: 50,
                              child: GestureDetector(
                                onTapDown: (_) => _sendInput('DPAD_UP', 1),
                                onTapUp: (_) => _sendInput('DPAD_UP', 0),
                                child: Container(
                                  width: 40,
                                  height: 40,
                                  decoration: BoxDecoration(
                                    color: Color(0xFF00B4DC),
                                    borderRadius: BorderRadius.circular(4),
                                  ),
                                  child: Icon(Icons.arrow_upward, size: 20),
                                ),
                              ),
                            ),
                            Positioned(
                              left: 0,
                              top: 50,
                              child: GestureDetector(
                                onTapDown: (_) => _sendInput('DPAD_LEFT', 1),
                                onTapUp: (_) => _sendInput('DPAD_LEFT', 0),
                                child: Container(
                                  width: 40,
                                  height: 40,
                                  decoration: BoxDecoration(
                                    color: Color(0xFF00B4DC),
                                    borderRadius: BorderRadius.circular(4),
                                  ),
                                  child: Icon(Icons.arrow_back, size: 20),
                                ),
                              ),
                            ),
                            Positioned(
                              right: 0,
                              top: 50,
                              child: GestureDetector(
                                onTapDown: (_) => _sendInput('DPAD_RIGHT', 1),
                                onTapUp: (_) => _sendInput('DPAD_RIGHT', 0),
                                child: Container(
                                  width: 40,
                                  height: 40,
                                  decoration: BoxDecoration(
                                    color: Color(0xFF00B4DC),
                                    borderRadius: BorderRadius.circular(4),
                                  ),
                                  child: Icon(Icons.arrow_forward, size: 20),
                                ),
                              ),
                            ),
                            Positioned(
                              bottom: 0,
                              left: 50,
                              child: GestureDetector(
                                onTapDown: (_) => _sendInput('DPAD_DOWN', 1),
                                onTapUp: (_) => _sendInput('DPAD_DOWN', 0),
                                child: Container(
                                  width: 40,
                                  height: 40,
                                  decoration: BoxDecoration(
                                    color: Color(0xFF00B4DC),
                                    borderRadius: BorderRadius.circular(4),
                                  ),
                                  child: Icon(Icons.arrow_downward, size: 20),
                                ),
                              ),
                            ),
                          ],
                        ),
                      ),
                      SizedBox(height: 48),

                      // Action Buttons
                      Row(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          _buildActionButton('Y', Colors.yellow, 'Y'),
                          _buildActionButton('X', Colors.blue, 'X'),
                          _buildActionButton('B', Colors.red, 'B'),
                          _buildActionButton('A', Colors.green, 'A'),
                        ],
                      ),
                      SizedBox(height: 32),

                      // Shoulder Buttons
                      Row(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          Column(
                            children: [
                              Text('L1'),
                              GestureDetector(
                                onTapDown: (_) => _sendInput('L1', 1),
                                onTapUp: (_) => _sendInput('L1', 0),
                                child: Container(
                                  width: 60,
                                  height: 40,
                                  decoration: BoxDecoration(
                                    color: Color(0xFF2D2D2D),
                                    borderRadius: BorderRadius.circular(8),
                                    border: Border.all(
                                      color: Color(0xFF00B4DC),
                                      width: 2,
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                          Column(
                            children: [
                              Text('R1'),
                              GestureDetector(
                                onTapDown: (_) => _sendInput('R1', 1),
                                onTapUp: (_) => _sendInput('R1', 0),
                                child: Container(
                                  width: 60,
                                  height: 40,
                                  decoration: BoxDecoration(
                                    color: Color(0xFF2D2D2D),
                                    borderRadius: BorderRadius.circular(8),
                                    border: Border.all(
                                      color: Color(0xFF00B4DC),
                                      width: 2,
                                    ),
                                  ),
                                ),
                              ),
                            ],
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
              ),
            )
          else
            Center(
              child: Text('Connect to device first'),
            ),
        ],
      ),
    );
  }

  Widget _buildSettingsTab() {
    return ListView(
      children: [
        ListTile(
          title: Text('Display'),
          trailing: Icon(Icons.chevron_right),
          onTap: () {},
        ),
        ListTile(
          title: Text('Sound'),
          trailing: Icon(Icons.chevron_right),
          onTap: () {},
        ),
        ListTile(
          title: Text('Network'),
          trailing: Icon(Icons.chevron_right),
          onTap: () {},
        ),
        ListTile(
          title: Text('Storage'),
          trailing: Icon(Icons.chevron_right),
          onTap: () {},
        ),
        ListTile(
          title: Text('About Device'),
          trailing: Icon(Icons.chevron_right),
          onTap: () {},
        ),
      ],
    );
  }

  Widget _buildQuickActionCard(String title, IconData icon, VoidCallback onTap) {
    return GestureDetector(
      onTap: onTap,
      child: Card(
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
        color: Color(0xFF1A1A1A),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(icon, size: 32, color: Color(0xFF00B4DC)),
            SizedBox(height: 8),
            Text(title),
          ],
        ),
      ),
    );
  }

  Widget _buildStatRow(String label, String value, double? progress) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 12.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(label, style: Theme.of(context).textTheme.bodySmall),
              Text(value, style: Theme.of(context).textTheme.bodySmall),
            ],
          ),
          if (progress != null) ...
            SizedBox(height: 4),
            ClipRRect(
              borderRadius: BorderRadius.circular(4),
              child: LinearProgressIndicator(
                value: progress,
                minHeight: 6,
              ),
            ),
        ],
      ),
    );
  }

  Widget _buildActionButton(String label, Color color, String button) {
    return GestureDetector(
      onTapDown: (_) => _sendInput(button, 1),
      onTapUp: (_) => _sendInput(button, 0),
      child: Container(
        width: 60,
        height: 60,
        decoration: BoxDecoration(
          color: color,
          shape: BoxShape.circle,
        ),
        child: Center(
          child: Text(
            label,
            style: TextStyle(
              color: Colors.white,
              fontWeight: FontWeight.bold,
              fontSize: 20,
            ),
          ),
        ),
      ),
    );
  }

  Future<void> _sendInput(String button, int state) async {
    if (connectedDeviceIp == null) return;

    try {
      await http.post(
        Uri.parse('http://$connectedDeviceIp:8888/api/v1/input/event'),
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode({
          'button': button,
          'state': state,
        }),
      );
    } catch (e) {
      // Error sending input
    }
  }
}
