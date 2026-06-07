#!/usr/bin/env python3
"""
autocopy.py - Monitor source and destination paths, copy when both are available.

Usage: python autocopy.py <source_path> <destination_path>

Waits for both the source file and the destination directory to exist,
then copies the source into the destination. Resets when either path
becomes unavailable so it can copy again on the next availability.

Example:
    python scripts/autocopy.py build/zephyr/zmk.uf2 /run/media/klesh/NICENANO
"""

import sys
import time
import shutil
import signal
import os

# Flag to control the main loop
running = True


def signal_handler(_sig, _frame):
    global running
    if running:
        print("\nExiting...")
        running = False
    else:
        sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)


def wait_and_copy(source_path, dest_path, check_interval=1.0):
    """Monitor source and dest paths, copy when both are valid."""
    global running

    source_path = os.path.expanduser(source_path)
    dest_path = os.path.expanduser(dest_path)

    if os.path.isfile(dest_path):
        print("Error: Destination is a file, expected a directory: " + dest_path)
        sys.exit(1)

    print("Monitoring...")
    print("  Source: " + source_path)
    print("  Destination: " + dest_path)
    print("Press Ctrl+C to exit.")

    already_copied = False

    while running:
        source_exists = os.path.exists(source_path)
        dest_exists = os.path.exists(dest_path)

        if source_exists and dest_exists:
            if not already_copied:
                try:
                    print("\nBoth paths available.")
                    dest_file = os.path.join(dest_path, os.path.basename(source_path))
                    print("Copying to: " + dest_file)
                    shutil.copy2(source_path, dest_file)
                    print("Copy complete.")
                    already_copied = True
                except Exception as e:
                    print("Copy failed: " + str(e))
        else:
            if already_copied:
                missing = []
                if not source_exists:
                    missing.append("source")
                if not dest_exists:
                    missing.append("destination")
                print("\n" + ", ".join(missing) + " unavailable. Waiting for both...")
                already_copied = False
            elif not already_copied:
                # Print waiting message only when transitioning to a missing state
                pass

        time.sleep(check_interval)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python autocopy.py <source_path> <destination_path>")
        print("")
        print("Example:")
        print("  python autocopy.py build/zephyr/zmk.uf2 /media/NICENANO")
        sys.exit(1)

    wait_and_copy(sys.argv[1], sys.argv[2])
