#!/usr/bin/env python3
import sys
import time
import glob
import signal
try:
    import serial
except ImportError:
    print("Error: 'pyserial' module is required. Install it using 'pip install pyserial'")
    sys.exit(1)

# Flag to control the main loop
running = True

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
    return sorted(glob.glob('/dev/tty.usbmodem*'))

def check_activity(port, timeout=2.0):
    print(f"Checking {port} for activity...")
    try:
        # Open with timeout
        with serial.Serial(port, baudrate=115200, timeout=0.1) as ser:
            start_time = time.time()
            while time.time() - start_time < timeout:
                if not running:
                    return False
                
                if ser.in_waiting > 0:
                    return True
                
                # Try reading a byte just in case in_waiting isn't reliable on some drivers
                data = ser.read(1)
                if data:
                    return True
                    
                time.sleep(0.1)
    except (OSError, serial.SerialException):
        # Don't print error for busy device (could be open by another tool)
        pass
    return False

def monitor_and_connect():
    global running
    print("--- USB Serial Monitor (pyserial) ---")
    print("Waiting for ZMK devices...")
    print("Press Ctrl+C to exit.")
    
    last_dev_count = 0

    while running:
        devices = get_zmk_devices()
        
        # Determine if device list changed
        if len(devices) != last_dev_count and len(devices) > 0:
             # Wait a moment for device to stabilize if it just appeared
             time.sleep(0.5) 
             
             print(f"\nFound {len(devices)} device(s): {', '.join(devices)}")
             
             target_dev = None
             
             # Check for activity to find the logging port
             for dev in devices:
                 if not running:
                     break
                 if check_activity(dev, timeout=2.0):
                     print(f"  -> Activity detected on {dev} (Logging Port)")
                     target_dev = dev
                     break
                 else:
                     print(f"  -> No immediate activity on {dev}")
            
             if not target_dev:
                 print("  -> No active output detected. Defaulting to first device.")
                 target_dev = devices[0]

             if target_dev and running:
                 print(f"\nConnecting to {target_dev}...")
                 try:
                     with serial.Serial(target_dev, 115200, timeout=0.1) as ser:
                         print("Connected! (Ctrl+C to exit)")
                         
                         while running:
                             try:
                                 # Read available data
                                 # We use a small timeout to keep the loop responsive to Ctrl+C
                                 data = ser.read(1024) 
                                 if data:
                                     sys.stdout.buffer.write(data)
                                     sys.stdout.buffer.flush()
                             except serial.SerialException:
                                 print("\nDevice disconnected.")
                                 break
                 except (OSError, serial.SerialException) as e:
                     print(f"Connection failed: {e}")
                 
                 last_dev_count = 0 # Force rescan logic
                 print("\nRescanning...")
                 time.sleep(1)
                 continue

        last_dev_count = len(devices)
        time.sleep(1)

if __name__ == "__main__":
    monitor_and_connect()
