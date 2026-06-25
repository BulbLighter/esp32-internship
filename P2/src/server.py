from flask import Flask, request, jsonify
import csv
import os
from datetime import datetime, timezone

app = Flask(__name__)

CSV_FILE = "readings.csv"
CSV_HEADERS = ["received_at", "ts", "temp", "hum"]

def ensure_csv_exists():
    if not os.path.exists(CSV_FILE):
        with open(CSV_FILE, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(CSV_HEADERS)

@app.route("/readings", methods=["POST"])
def receive_reading():
    data = request.get_json(silent=True)

    if data is None:
        return jsonify({"status": "error", "message": "invalid or missing JSON"}), 400

    required_keys = {"temp", "hum", "ts"}
    if not required_keys.issubset(data.keys()):
        return jsonify({"status": "error", "message": f"missing keys, need {required_keys}"}), 400

    received_at = datetime.now(timezone.utc).isoformat()

    ensure_csv_exists()
    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([received_at, data["ts"], data["temp"], data["hum"]])

    print(f"[{received_at}] Logged: temp={data['temp']}, hum={data['hum']}, ts={data['ts']}")

    return jsonify({"status": "ok"}), 200

if __name__ == "__main__":
    ensure_csv_exists()
    app.run(host="0.0.0.0", port=5000, debug=False)
    # for multi hour run we switch from debug true to false, 
    # otherwise the server will restart every time a file is changed