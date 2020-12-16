#!/usr/bin/env python3

import socket
import time
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('-a',dest="a", type=str, default="127.0.0.1")
parser.add_argument('-p',dest="p", type=int, default=5757)


args = parser.parse_args()

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((args.a, args.p))

    for line in open("AAPL.csv", "r").readlines():
        line = bytes("{}\0".format(line), encoding="utf-8")
        s.send(line)
        print(line)
        time.sleep(1)