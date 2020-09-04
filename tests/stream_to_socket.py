#!/usr/bin/env python3

import socket
import time

HOST = '127.0.0.1'
PORT = 5757

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    for line in open("AAPL.csv", "r").readlines():
        line = bytes("{}\0".format(line), encoding="utf-8")
        s.send(line)
        print(line)
        time.sleep(1)