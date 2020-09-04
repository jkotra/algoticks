#   @author: Jagadeesh Kotra


import time
import datetime
import time


while True:
    try:
        csv_file = open("../bin/debug/example.csv", "a")
        for line in open("AAPL.csv", "r").readlines():

            csv_file.write(line);
            time.sleep(1)

    except KeyboardInterrupt:
        print("Bye!")
        exit(0)