#   @author: Jagadeesh Kotra


import time
import datetime
import time


while True:
    try:
        for line in open("AAPL.csv", "r").readlines():
            csv_file = open("../bin/debug/example.csv", "a")
            csv_file.write(line)
            csv_file.close()
            time.sleep(1)

    except KeyboardInterrupt:
        print("Bye!")
        exit(0)