#!/usr/bin/env python3
import argparse
import serial
import sys
import time

import threading
from termcolor import colored

PACKET_LENGTH = 4
PACKET_TIMEOUT = 0.1  # One packet is 4 bytes ~ 4.2 ms @ 9600 bps (1 / 9600 * 10 * 4)

START_BYTE = 0xA5

# Commands

CMD_TEMP_REPORT = 0x06
CMD_FLOW_REPORT = 0x08

CMD_SET_FILTER = 0x02
CMD_SET_BUBBLE = 0x03
CMD_SET_TARGET_TEMP = 0x04
CMD_SET_OZONE = 0x0E
CMD_SET_UVC = 0x15


def _hex_str(data):
    return " ".join([f"{d:02X}" for d in data])


def _decode_packet(data):
    packet_info = f"{_hex_str(data)}:"
    check_sum = sum(data[:-1]) % 256
    if check_sum != data[-1]:
        packet_info += (
            f" Invalid checksum, was 0x{data[-1]:02X}, calculated 0x{check_sum:02X}"
        )

    command = data[1]

    if command == CMD_TEMP_REPORT:
        temperature = data[2] / 2
        packet_info += f" Temp report: {temperature} °C"
    elif command == CMD_FLOW_REPORT:
        flow_input = data[2] & 0x01 != 0
        flow_output = data[2] & 0x02 != 0
        if data[2] not in [0, 1, 2, 3]:
            packet_info += f"Unknown flow report: {data[2]}"
        packet_info += f" Flow report: In: {flow_input}, out: {flow_output}"
    elif command == CMD_SET_TARGET_TEMP:
        target_temperature = data[2]
        packet_info += f" Set temp: {target_temperature} °C"
    elif command == CMD_SET_BUBBLE:
        speed = data[2]
        packet_info += f" Set bubble speed: {speed}"
    elif command == CMD_SET_FILTER:
        if data[2] == 0x01:
            enabled = True
        elif data[2] == 0x00:
            enabled = False
        else:
            print("Unknown value!", data[2])
        packet_info += f" Set filter: {enabled}"
    elif command == CMD_SET_UVC:
        enabled = data[2] == 0x01
        if data[2] not in [0, 1]:
            packet_info += f"Unknown UVC report: {data[2]}"
        packet_info += f" Set UVC: {enabled}"
    elif command == CMD_SET_OZONE:
        enabled = data[2] == 0x01
        if data[2] not in [0, 1]:
            packet_info += f"Unknown ozone report: {data[2]}"
        packet_info += f" Ozone enabled: {enabled}"
    else:
        packet_info += f" Unknown command {_hex_str(data)}"

    return packet_info


def _read_start_byte(port):
    while True:
        data = port.read(1)

        if data is not None and len(data) == 1:
            if data[0] == START_BYTE:
                return data
            else:
                print(f"Invalid start: {_hex_str(data)}")


class PacketReader(threading.Thread):
    def __init__(self, port: str = None, color=None):
        super().__init__(name=f"PacketReader {port}", daemon=True)
        self._stop_event = threading.Event()
        self._port = port
        self._ser = None
        self._color = color

    def run(self):
        with serial.Serial(
            self._port,
            baudrate=9600,
            bytesize=8,
            parity="N",
            stopbits=1,
            exclusive=True,
            timeout=PACKET_TIMEOUT,
        ) as self._ser:
            known_packets = {}
            start = time.time()

            while not self._stop_event.is_set():
                data = _read_start_byte(self._ser)

                data += self._ser.read(PACKET_LENGTH - 1)

                if len(data) != PACKET_LENGTH:
                    print(
                        f"Incomplete packet: {_hex_str(data)}, len={len(data)}, data={data}"
                    )
                    continue

                packet_str = _decode_packet(data)
                if data[1] not in known_packets:
                    known_packets[data[1]] = packet_str
                    print(
                        colored(
                            f"{self._port}: {time.time() - start:03.2f}: New packet: {packet_str}",
                            self._color,
                        )
                    )
                else:
                    print(
                        colored(
                            f"{self._port}: {time.time() - start:03.2f}: Old packet: {packet_str}",
                            self._color,
                        )
                    )

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._stop_event.set()
        self._ser.cancel_read()
        self.join()


def main():
    parser = argparse.ArgumentParser(
        description="Read and decode M-SPA packets from UART"
    )
    parser.add_argument("--port1", default="/dev/ttyUSB0")
    parser.add_argument("--port2", default="/dev/ttyUSB1")
    args = parser.parse_args()

    print("exit to exit")
    with (
        PacketReader(args.port1, "green") as _reader1,
        PacketReader(args.port2, "red") as _reader2,
    ):
        while input("") != "exit":
            print("----------------")

    return 0


if __name__ == "__main__":
    sys.exit(main())
