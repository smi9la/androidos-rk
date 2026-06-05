import React, { useState } from 'react';
import './RemoteControl.css';

const RemoteControl = ({ device }) => {
  const [controlMode, setControlMode] = useState('gamepad');
  const [sensitivity, setSensitivity] = useState(50);
  const [latency, setLatency] = useState(0);

  const handleButtonPress = (button) => {
    console.log(`Button pressed: ${button}`);
    // Send to device via IPC
  };

  const handleAnalogStick = (x, y) => {
    console.log(`Analog stick: ${x}, ${y}`);
    // Send to device via IPC
  };

  const handleTrigger = (trigger, value) => {
    console.log(`Trigger ${trigger}: ${value}`);
    // Send to device via IPC
  };

  return (
    <div className="remote-control">
      <div className="control-header">
        <h2>Remote Controller</h2>
        <div className="connection-status">
          <span>Connected to: {device}</span>
          <span className="latency">Latency: {latency}ms</span>
        </div>
      </div>

      <div className="control-container">
        {/* Left Side - D-Pad and Left Analog */}
        <div className="left-controls">
          <div className="dpad">
            <button
              className="dpad-btn up"
              onClick={() => handleButtonPress('up')}
              onMouseDown={() => setLatency(Math.random() * 100)}
            >
              ▲
            </button>
            <button className="dpad-btn left" onClick={() => handleButtonPress('left')}>
              ◀
            </button>
            <button className="dpad-btn right" onClick={() => handleButtonPress('right')}>
              ▶
            </button>
            <button className="dpad-btn down" onClick={() => handleButtonPress('down')}>
              ▼
            </button>
          </div>

          <div className="analog-stick">
            <div className="stick-bg">
              <div className="stick-thumb" />
            </div>
          </div>
        </div>

        {/* Center - Screen Display */}
        <div className="center-display">
          <div className="phone-screen">
            <div className="screen-content">Game Screen Stream</div>
          </div>
        </div>

        {/* Right Side - Action Buttons and Right Analog */}
        <div className="right-controls">
          <div className="action-buttons">
            <button
              className="btn y-btn"
              onClick={() => handleButtonPress('Y')}
              title="Y"
            >
              Y
            </button>
            <button
              className="btn x-btn"
              onClick={() => handleButtonPress('X')}
              title="X"
            >
              X
            </button>
            <button
              className="btn b-btn"
              onClick={() => handleButtonPress('B')}
              title="B"
            >
              B
            </button>
            <button
              className="btn a-btn"
              onClick={() => handleButtonPress('A')}
              title="A"
            >
              A
            </button>
          </div>

          <div className="analog-stick right">
            <div className="stick-bg">
              <div className="stick-thumb" />
            </div>
          </div>
        </div>
      </div>

      {/* Bottom - Triggers and Mode */}
      <div className="bottom-controls">
        <div className="trigger-group">
          <div className="trigger l1" onMouseDown={() => handleTrigger('L1', 1)}>
            L1
          </div>
          <div className="trigger l2" onMouseDown={() => handleTrigger('L2', 1)}>
            L2
          </div>
        </div>

        <div className="center-buttons">
          <button className="btn select-btn" onClick={() => handleButtonPress('SELECT')}>
            SELECT
          </button>
          <button className="btn start-btn" onClick={() => handleButtonPress('START')}>
            START
          </button>
          <button className="btn home-btn" onClick={() => handleButtonPress('HOME')}>
            🏠
          </button>
        </div>

        <div className="trigger-group right">
          <div className="trigger r1" onMouseDown={() => handleTrigger('R1', 1)}>
            R1
          </div>
          <div className="trigger r2" onMouseDown={() => handleTrigger('R2', 1)}>
            R2
          </div>
        </div>
      </div>

      {/* Settings */}
      <div className="control-settings">
        <div className="setting-group">
          <label>Mode:</label>
          <select value={controlMode} onChange={(e) => setControlMode(e.target.value)}>
            <option value="gamepad">Gamepad</option>
            <option value="touch">Touch Screen</option>
            <option value="keyboard">Keyboard</option>
          </select>
        </div>
        <div className="setting-group">
          <label>Sensitivity:</label>
          <input
            type="range"
            min="0"
            max="100"
            value={sensitivity}
            onChange={(e) => setSensitivity(e.target.value)}
          />
          <span>{sensitivity}%</span>
        </div>
        <button className="btn calibrate-btn">Calibrate</button>
        <button className="btn settings-btn">Settings</button>
      </div>
    </div>
  );
};

export default RemoteControl;
