#!/usr/bin/env python

import Adafruit_BMP.BMP085 as BMP085
import os
import datetime
import sqlite3

# store the temperature in the database
def log_temperature(temp,pres):

    conn=sqlite3.connect('/var/www/my_log.db')
    curs=conn.cursor()
    curs.execute("INSERT INTO temps values(date('now','localtime'),time('now', 'localtime'), (?), (?))", (temp,pres/100))

    # commit the changes
    conn.commit()

    conn.close()

def display_data():

    conn=sqlite3.connect('/var/www/my_log.db')
    curs=conn.cursor()

    for row in curs.execute("SELECT * FROM temps"):
        print str(row[0])+"	"+str(row[1]+ " " + str(row[2]) + " " + str(row[3]))

    conn.close()


sensor = BMP085.BMP085()
temp=sensor.read_temperature()
pres=sensor.read_pressure()
log_temperature(temp,pres)
display_data()
