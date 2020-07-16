#
#   @author: Jagadeesh Kotra
#   @desc: this script will continuously write fake rows to ../bin/debug/example.csv.
#          this tool can help debugging/testing live mode(-L) in algoticks.
#
#

from time import sleep
import datetime
import time


while True:
    try:
        csv_file = open("../bin/debug/example.csv", "a")
        now_dt = str(datetime.datetime.fromtimestamp(time.time()))
        csv_file.write(
        "{now_dt},437.75,438,437.75,438,4944,4944,438,438,438\n".format(now_dt=now_dt))
        print("{} row writtten to file!", now_dt)
        csv_file.close()
        sleep(1)
    except KeyboardInterrupt:
        print("Bye!")
        exit(0)