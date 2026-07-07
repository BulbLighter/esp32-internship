from flask import Flask, request, jsonify, render_template_string
import csv
import os
from datetime import datetime

app = Flask(__name__)
CSV_FILE = "readings.csv"

# Create CSV with headers if it doesn't exist
if not os.path.exists(CSV_FILE):
    with open(CSV_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['timestamp', 'device_id', 'fw_version', 'uptime_s',
                         'rssi', 'temp', 'hum', 'light_raw', 'light_lux_approx'])

@app.route('/readings', methods=['POST'])
def readings():
    data = request.get_json()
    if not data:
        return jsonify({"status": "error", "msg": "no data"}), 400

    with open(CSV_FILE, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            data.get('device_id', ''),
            data.get('fw_version', ''),
            data.get('uptime_s', ''),
            data.get('rssi', ''),
            data.get('temp', ''),
            data.get('hum', ''),
            data.get('light_raw', ''),
            data.get('light_lux_approx', '')
        ])

    return jsonify({"status": "ok"}), 200

DASHBOARD_HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Dashboard</title>
    <meta http-equiv="refresh" content="15">
    <style>
        body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee; padding: 20px; }
        h1 { color: #00d4ff; }
        .card { background: #16213e; border-radius: 10px; padding: 20px; 
                margin: 10px 0; display: inline-block; width: 150px; text-align: center; }
        .value { font-size: 2em; color: #00d4ff; font-weight: bold; }
        .label { font-size: 0.9em; color: #aaa; margin-top: 5px; }
        table { border-collapse: collapse; width: 100%; margin-top: 20px; }
        th { background: #0f3460; padding: 10px; text-align: left; }
        td { padding: 8px 10px; border-bottom: 1px solid #333; }
        tr:hover { background: #16213e; }
    </style>
</head>
<body>
    <h1>ESP32 Sensor Dashboard</h1>
    <p style="color:#aaa">Auto-refreshes every 15 seconds</p>

    {% if latest %}
    <div>
        <div class="card">
            <div class="value">{{ latest.temp }}°C</div>
            <div class="label">Temperature</div>
        </div>
        <div class="card">
            <div class="value">{{ latest.hum }}%</div>
            <div class="label">Humidity</div>
        </div>
        <div class="card">
            <div class="value">{{ latest.light_raw }}</div>
            <div class="label">Light Raw</div>
        </div>
        <div class="card">
            <div class="value">{{ latest.rssi }} dBm</div>
            <div class="label">RSSI</div>
        </div>
        <div class="card">
            <div class="value">{{ latest.uptime_s }}s</div>
            <div class="label">Uptime</div>
        </div>
    </div>
    {% endif %}

    <h2 style="margin-top:30px">Recent Readings</h2>
    <table>
        <tr>
            <th>Timestamp</th><th>Temp</th><th>Humidity</th>
            <th>Light Raw</th><th>Light Lux</th><th>RSSI</th><th>Uptime</th>
        </tr>
        {% for row in rows %}
        <tr>
            <td>{{ row.timestamp }}</td>
            <td>{{ row.temp }}°C</td>
            <td>{{ row.hum }}%</td>
            <td>{{ row.light_raw }}</td>
            <td>{{ row.light_lux_approx }}</td>
            <td>{{ row.rssi }} dBm</td>
            <td>{{ row.uptime_s }}s</td>
        </tr>
        {% endfor %}
    </table>
</body>
</html>
"""

@app.route('/dashboard', methods=['GET'])
def dashboard():
    rows = []
    if os.path.exists(CSV_FILE):
        with open(CSV_FILE, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                rows.append(row)
    rows = rows[-20:]  # last 20 readings
    rows.reverse()
    latest = rows[0] if rows else None
    return render_template_string(DASHBOARD_HTML, rows=rows, latest=latest)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)