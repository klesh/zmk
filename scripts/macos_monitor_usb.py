#!/usr/bin/env python3
import time
import glob
import subprocess
import sys


def monitor_and_connect():
    print("--- USB Serial Monitor ---")
    print("Waiting for devices matching '/dev/tty.usbmodem*'...")
    print("Press Ctrl+C to exit.")
    
    last_dev_count = 0

    while True:
        try:
            # Find matching devices
            devices = sorted(glob.glob('/dev/tty.usbmodem*'))
            
            if not devices:
                if last_dev_count > 0:
                     print("\nNo devices found. Waiting...", end='\r')
                last_dev_count = 0
                time.sleep(1)
                continue
            
            if len(devices) != last_dev_count:
                print(f"\nFound {len(devices)} device(s):")
                for i, dev in enumerate(devices):
                    label = " (Likely Logging)" if i == 0 else " (Likely Studio/RPC)" if i == 1 else ""
                    print(f"  - {dev}{label}")
                last_dev_count = len(devices)

            # Iterate through devices
            connected_any = False
            for dev in devices:
                print(f"Connecting to {dev} using 'cu -l {dev}'...")
                
                try:
                    subprocess.run(['cu', '-l', dev], check=True)
                    print(f"\nConnection to {dev} closed.")
                    connected_any = True
                    break 
                    
                except subprocess.CalledProcessError:
                    print(f"Connection to {dev} failed. Trying next device...")
                    continue
                except FileNotFoundError:
                    print("Error: 'cu' command not found. Please ensure it is installed.")
                    return

            if not connected_any:
                time.sleep(2)

                
        except KeyboardInterrupt:
            print("\nExiting monitor.")
            sys.exit(0)
        except Exception as e:
            print(f"\nUnexpected error: {e}")
            time.sleep(1)

if __name__ == "__main__":
    monitor_and_connect()
