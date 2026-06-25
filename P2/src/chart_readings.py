import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

# Load data
df = pd.read_csv("readings.csv")

# Parse received_at as actual datetime (this is the real wall-clock time, unlike ts)
df["received_at"] = pd.to_datetime(df["received_at"])

# Sort chronologically just in case (should already be in order, but safe to enforce)
df = df.sort_values("received_at").reset_index(drop=True)

# Create figure with two y-axes (temp and humidity have different scales/units)
fig, ax1 = plt.subplots(figsize=(14, 6))

color_temp = "tab:red"
ax1.set_xlabel("Time")
ax1.set_ylabel("Temperature (°C)", color=color_temp)
ax1.plot(df["received_at"], df["temp"], color=color_temp, label="Temperature", linewidth=1.2)
ax1.tick_params(axis="y", labelcolor=color_temp)

ax2 = ax1.twinx()
color_hum = "tab:blue"
ax2.set_ylabel("Humidity (%)", color=color_hum)
ax2.plot(df["received_at"], df["hum"], color=color_hum, label="Humidity", linewidth=1.2)
ax2.tick_params(axis="y", labelcolor=color_hum)

# Format x-axis to show readable timestamps
ax1.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
fig.autofmt_xdate()

plt.title(f"DHT22 Readings: {df['received_at'].iloc[0].strftime('%Y-%m-%d %H:%M')} to {df['received_at'].iloc[-1].strftime('%H:%M')}")
fig.tight_layout()

plt.savefig("temp_humidity_chart.png", dpi=150)
plt.show()

# Print some summary stats for the write-up
print(f"Total readings: {len(df)}")
print(f"Duration: {df['received_at'].iloc[-1] - df['received_at'].iloc[0]}")
print(f"Temp range: {df['temp'].min()}°C to {df['temp'].max()}°C")
print(f"Humidity range: {df['hum'].min()}% to {df['hum'].max()}%")