# server_p3.py — Standalone Flask Server for Project 3 (LDR Light Sensor)


from flask import Flask, request, jsonify
import csv
from datetime import datetime
import os

app = Flask(__name__)

LIGHT_CSV = 'light_readings.csv'


@app.route('/light', methods=['POST'])
def receive_light_reading():
    data = request.get_json()
    
    light_raw = data.get('light_raw')
    light_volts = data.get('light_volts')
    ts = data.get('ts')
    received_at = datetime.now().isoformat()
    
    file_exists = os.path.isfile(LIGHT_CSV)
    
    with open(LIGHT_CSV, 'a', newline='') as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(['received_at', 'light_raw', 'light_volts', 'esp32_ts'])
        writer.writerow([received_at, light_raw, light_volts, ts])
    
    print(f"[LIGHT] raw={light_raw}, volts={light_volts}, ts={ts}")
    
    return jsonify({"status": "ok"}), 200


@app.route('/', methods=['GET'])
def home():
    return jsonify({
        "status": "Project 3 Flask server running",
        "endpoint": "/light (POST)"
    }), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)