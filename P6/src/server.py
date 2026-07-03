from flask import Flask, request, jsonify
from datetime import datetime
import csv
import os

app = Flask(__name__)

CSV_FILE = "pulse_data.csv"

# Create CSV with headers if it doesn't exist
if not os.path.exists(CSV_FILE):
    with open(CSV_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "count_total", "rate_per_min"])

@app.route("/pulses", methods=["POST"])
def receive_pulse_data():
    data = request.get_json()

    if not data or "count_total" not in data or "rate_per_min" not in data:
        return jsonify({"status": "error", "message": "missing fields"}), 400

    count_total = data["count_total"]
    rate_per_min = data["rate_per_min"]
    timestamp = datetime.now().isoformat()

    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([timestamp, count_total, rate_per_min])

    print(f"[{timestamp}] count_total={count_total} rate_per_min={rate_per_min}")

    return jsonify({"status": "ok"}), 200

@app.route("/pulses", methods=["GET"])
def view_latest():
    # Quick way to see live data in browser: http://<PC-IP>:5000/pulses
    if not os.path.exists(CSV_FILE):
        return jsonify({"status": "no data yet"})
    with open(CSV_FILE, "r") as f:
        lines = f.readlines()
    last_10 = lines[-10:] if len(lines) > 10 else lines
    return "<br>".join(last_10)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)


# [2026-07-04T03:20:25.659484] count_total=282 rate_per_min=0.0
# 172.22.15.18 - - [04/Jul/2026 03:20:25] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:20:55.908432] count_total=614 rate_per_min=611.0
# 172.22.15.18 - - [04/Jul/2026 03:20:55] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:21:25.977872] count_total=945 rate_per_min=611.0
# 172.22.15.18 - - [04/Jul/2026 03:21:25] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:21:56.389184] count_total=1278 rate_per_min=661.0
# 172.22.15.18 - - [04/Jul/2026 03:21:56] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:22:26.464506] count_total=1613 rate_per_min=661.0
# 172.22.15.18 - - [04/Jul/2026 03:22:26] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:22:56.743807] count_total=1950 rate_per_min=666.0
# 172.22.15.18 - - [04/Jul/2026 03:22:56] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:23:27.055015] count_total=2286 rate_per_min=666.0
# 172.22.15.18 - - [04/Jul/2026 03:23:27] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:23:57.162220] count_total=2624 rate_per_min=667.0
# 172.22.15.18 - - [04/Jul/2026 03:23:57] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:24:27.526237] count_total=2963 rate_per_min=667.0
# 172.22.15.18 - - [04/Jul/2026 03:24:27] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:24:57.600896] count_total=3301 rate_per_min=668.0
# 172.22.15.18 - - [04/Jul/2026 03:24:57] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:25:27.890018] count_total=3632 rate_per_min=668.0
# 172.22.15.18 - - [04/Jul/2026 03:25:27] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:25:57.967236] count_total=3966 rate_per_min=666.0
# 172.22.15.18 - - [04/Jul/2026 03:25:57] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:26:28.043492] count_total=4282 rate_per_min=666.0
# 172.22.15.18 - - [04/Jul/2026 03:26:28] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:26:58.108139] count_total=4587 rate_per_min=618.0
# 172.22.15.18 - - [04/Jul/2026 03:26:58] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:27:28.317994] count_total=4921 rate_per_min=618.0
# 172.22.15.18 - - [04/Jul/2026 03:27:28] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:27:58.423711] count_total=5257 rate_per_min=668.0
# 172.22.15.18 - - [04/Jul/2026 03:27:58] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:28:28.529895] count_total=5589 rate_per_min=668.0
# 172.22.15.18 - - [04/Jul/2026 03:28:28] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:28:58.885660] count_total=5923 rate_per_min=661.0
# 172.22.15.18 - - [04/Jul/2026 03:28:58] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:29:29.156039] count_total=6265 rate_per_min=661.0
# 172.22.15.18 - - [04/Jul/2026 03:29:29] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:29:59.485931] count_total=6606 rate_per_min=676.0
# 172.22.15.18 - - [04/Jul/2026 03:29:59] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:30:29.550128] count_total=6941 rate_per_min=676.0
# 172.22.15.18 - - [04/Jul/2026 03:30:29] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:30:59.676410] count_total=7273 rate_per_min=665.0
# 172.22.15.18 - - [04/Jul/2026 03:30:59] "POST /pulses HTTP/1.1" 200 -
# [2026-07-04T03:31:29.754494] count_total=7605 rate_per_min=665.0
# 172.22.15.18 - - [04/Jul/2026 03:31:29] "POST /pulses HTTP/1.1" 200 -