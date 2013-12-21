#!/usr/bin/env python
import RPi.GPIO as io
import time

PWR_BTN = 16
BATH_BTN = 15


def push(btn):
    io.setmode(io.BOARD)
    io.setup(btn, io.OUT)
    io.output(btn, 1)
    time.sleep(0.1)
    io.output(btn, 0)
    io.setup(btn, io.IN)


def fill():
    push(PWR_BTN)
    time.sleep(0.1)
    push(BATH_BTN)


def stop():
    push(PWR_BTN)


if __name__ == '__main__':
    import sys
    command = sys.argv[1]

    if command == 'fill':
        fill()
    elif command == 'stop':
        stop()