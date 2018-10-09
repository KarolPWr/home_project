#!/usr/bin/env python

import Adafruit_BMP.BMP085 as BMP085
import os
import datetime
import sqlite3
import glob
import time

# store the temperature in the database
def log_temperature(temp):
    
    temp = temp
    conn=sqlite3.connect('/var/www/templog.db')
    curs=conn.cursor()
    curs.execute("INSERT INTO temps values(datetime('now', 'localtime'), (?))", (temp,))
    # commit the changes
    conn.commit()

    conn.close()


def log_pressure(pressure):

    pressure = pressure/100
    conn=sqlite3.connect('/var/www/pressurelog.db')
    curs=conn.cursor()
    curs.execute("INSERT INTO temps values(datetime('now', 'localtime'), (?))", (pressure,))
    # commit the changes
    conn.commit()
    print "Logging pressure done"

    conn.close()



def display_data():

    conn=sqlite3.connect('/var/www/templog.db')
    curs=conn.cursor()

    for row in curs.execute("SELECT * FROM temps"):
        print str(row[0])+"	"+str(row[1])

    conn.close()

def read_temp_raw():
    os.system('modprobe w1-gpio')
    os.system('modprobe w1-therm')
 
    base_dir = '/sys/bus/w1/devices/'
    device_folder = glob.glob(base_dir + '28*')[0]
    device_file = device_folder + '/w1_slave'
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines

def read_temp():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        return temp_c

#pressure=sensor.read_pressure()
log_temperature(read_temp())
#log_pressure(pressure)
display_data()
