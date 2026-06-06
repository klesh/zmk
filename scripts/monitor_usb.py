#!/usr/bin/env python3
import sys
import time
import glob
import signal
import threading
from collections import defaultdict

try:
    import serial
except ImportError:
    print("Error: 'pyserial' module is required. Install it using 'pip install pyserial'")
    sys.exit(1)

# Flag to control the main loop
running = True

# ANSI color codes for device output
COLORS = [
    "\033[36m",   # Cyan
    "\033[33m",   # Yellow
    "\033[35m",   # Magenta
    "\033[32m",   # Green
    "\033[34m",   # Blue
    "\033[31m",   # Red
]
COLOR_RESET = "\033[0m"


def signal_handler(sig, frame):
    global running
    if running:
        print("\nExiting...")
        running = False
    else:
        # Force exit if stuck
        sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)


def get_zmk_devices():
    """Get available ZMK USB serial devices (cross-platform)."""
    if sys.platform == 'darwin':
        # macOS: /dev/tty.usbmodem*
        return sorted(glob.glob('/dev/tty.usbmodem*'))
    elif sys.platform == 'linux':
        # Linux: /dev/ttyUSB* and /dev/ttyACM*
        devices = sorted(glob.glob('/dev/ttyUSB*')) + sorted(glob.glob('/dev/ttyACM*'))
        return sorted(set(devices))  # Remove duplicates and sort
    elif sys.platform == 'win32':
        # Windows: COM* ports
        return sorted(glob.glob('COM*'))
    else:
        # Fallback for other Unix-like systems
        return sorted(glob.glob('/dev/tty*'))


def monitor_device(port, color, print_lock):
    """Monitor a single device and print its output with device path prefix."""
    global running

    # Extract just the device name for cleaner prefix
    dev_name = port.split('/')[-1] if '/' in port else port
    prefix = f"[{dev_name}]"

    try:
        with serial.Serial(port, 115200, timeout=0.1) as ser:
            with print_lock:
                print(f"{color}{prefix} Connected{COLOR_RESET}")

            # Buffer to accumulate partial lines
            line_buffer = ""

            while running:
                try:
                    data = ser.read(1024)
                    if not data:
                        continue

                    # Decode bytes to string, handling partial UTF-8
                    text = data.decode('utf-8', errors='replace')

                    # Process text line by line
                    lines = text.split('\n')

                    # First chunk continues the buffered line
                    line_buffer += lines[0]

                    # If text ends with newline, line_buffer is complete
                    # Otherwise it stays buffered
                    if len(lines) > 1:
                        # Print completed line
                        with print_lock:
                            print(f"{color}{prefix} {line_buffer}{COLOR_RESET}")

                        # Print all complete middle lines
                        for line in lines[1:-1]:
                            with print_lock:
                                print(f"{color}{prefix} {line}{COLOR_RESET}")

                        # Last chunk (may be partial or empty if text ends with newline)
                        line_buffer = lines[-1]

                except serial.SerialException:
                    with print_lock:
                        print(f"{color}{prefix} Device disconnected.{COLOR_RESET}")
                    break

    except (OSError, serial.SerialException) as e:
        with print_lock:
            print(f"{color}{prefix} Connection failed: {e}{COLOR_RESET}")
        return

    with print_lock:
        print(f"{color}{prefix} Stopped monitoring.{COLOR_RESET}")


def monitor_and_connect():
    global running
    print("--- USB Serial Monitor (pyserial) ---")
    print(f"Platform: {sys.platform}")
    print("Waiting for ZMK devices...")
    print("Press Ctrl+C to exit.")

    print_lock = threading.Lock()
    active_threads = {}
    device_color_map = {}
    color_index = 0

    while running:
        devices = get_zmk_devices()

        # Start monitoring new devices
        for dev in devices:
            if dev not in active_threads or not active_threads[dev].is_alive():
                # Assign color if new device
                if dev not in device_color_map:
                    device_color_map[dev] = COLORS[color_index % len(COLORS)]
                    color_index += 1

                with print_lock:
                    color = device_color_map[dev]
                    dev_name = dev.split('/')[-1] if '/' in dev else dev
                    colored_name = f"{color}{dev_name}{COLOR_RESET}"
                    print(f"\nFound new device: {colored_name}")

                thread = threading.Thread(
                    target=monitor_device,
                    args=(dev, device_color_map[dev], print_lock),
                    daemon=True
                )
                active_threads[dev] = thread
                thread.start()

        # Clean up finished threads
        for dev in list(active_threads.keys()):
            if not active_threads[dev].is_alive():
                del active_threads[dev]
                # Remove color mapping to allow reuse, but keep index advancing
                # for visual distinction between sessions

        time.sleep(1)

    # Wait for all threads to finish (they'll exit because running=False)
    for dev, thread in active_threads.items():
        thread.join(timeout=2.0)


if __name__ == "__main__":
    monitor_and_connect()
