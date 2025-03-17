#!/usr/bin/env python3
import argparse
import serial
import sys
import time

PACKET_LENGTH = 4
PACKET_TIMEOUT = 0.1  # One packet is 4 bytes ~ 4.2 ms @ 9600 bps (1 / 9600 * 10 * 4)

START_BYTE = 0xA5

# Commands

CMD_TEMP_REPORT = 0x06
CMD_SET_TARGET_TEMP = 0x04


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
    elif command == CMD_SET_TARGET_TEMP:
        target_temperature = data[2]
        packet_info += f" Set temp: {target_temperature} °C"
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


def _read_uart(port):
    print(f"Reading packets from {port}")

    with serial.Serial(
        port,
        baudrate=9600,
        bytesize=8,
        parity="N",
        stopbits=1,
        exclusive=True,
        timeout=PACKET_TIMEOUT,
    ) as port:
        known_packets = {}

        start = time.time()
        while True:
            data = _read_start_byte(port)

            data += port.read(PACKET_LENGTH - 1)

            if len(data) != PACKET_LENGTH:
                print(
                    f"Incomplete packet: {_hex_str(data)}, len={len(data)}, data={data}"
                )
                continue

            packet_str = _decode_packet(data)
            if packet_str not in known_packets:
                known_packets[packet_str] = packet_str
                print(f"{time.time() - start:03.2f}: New packet: {packet_str}")
            else:
                print(f"{time.time() - start:03.2f}: Old packet: {packet_str}")


def main():
    parser = argparse.ArgumentParser(
        description="Read and decode M-SPA packets from UART"
    )
    parser.add_argument("--port", default="/dev/ttyUSB0")
    args = parser.parse_args()
    _read_uart(args.port)

    return 0


if __name__ == "__main__":
    sys.exit(main())
