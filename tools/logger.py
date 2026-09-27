"""
TapLock serial logger.

Reads the CSV lines that taplock.ino prints and appends them to
data/attempts.csv, with the date and time of the computer added in front.

    python3 -m pip install -r tools/requirements.txt
    python3 tools/logger.py                          list the serial ports
    python3 tools/logger.py /dev/cu.usbmodem14101    macOS
    python3 tools/logger.py COM3                     Windows

Stop with Ctrl+C. Close the Arduino serial monitor first, only one program
can use the port at a time.
"""

import csv
import sys
from datetime import datetime
from pathlib import Path

import serial
from serial.tools import list_ports

BAUD = 9600
OUT = Path(__file__).resolve().parent.parent / "data" / "attempts.csv"
HEADER = ["datetime", "millis", "uid", "result"]


def print_ports() -> None:
    ports = list_ports.comports()
    if not ports:
        print("No serial ports found. Is the Arduino plugged in?")
    for p in ports:
        print(f"  {p.device}  {p.description}")


def main() -> None:
    if len(sys.argv) < 2:
        print(__doc__)
        print_ports()
        sys.exit(1)

    port = sys.argv[1]
    new_file = not OUT.exists() or OUT.stat().st_size == 0

    try:
        ser = serial.Serial(port, BAUD, timeout=1)
    except serial.SerialException as e:
        print(f"Cannot open {port}: {e}")
        print_ports()
        sys.exit(1)

    with ser, OUT.open("a", newline="") as fh:
        writer = csv.writer(fh)
        if new_file:
            writer.writerow(HEADER)

        print(f"Listening on {port}, writing to {OUT}")
        while True:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            # skip empty lines, the header the Arduino prints and status messages
            if not line or line.startswith(("millis", "#")):
                continue

            parts = line.split(",")
            if len(parts) != 3:
                continue

            row = [datetime.now().isoformat(timespec="seconds")] + parts
            writer.writerow(row)
            fh.flush()
            print(",".join(row))


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.")
