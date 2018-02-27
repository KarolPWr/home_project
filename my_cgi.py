#!/usr/bin/env python

import Adafruit_BMP.BMP085 as BMP085
import cgi
import cgitb
import os
import sqlite3


def get_data():
        conn=sqlite3.connect('/var/www/my_log.db')
        curs=conn.cursor()
        curs.execute("SELECT * FROM temps ORDER BY ttime DESC LIMIT 1")
        result = curs.fetchone()
        return result

cgitb.enable()
sensor = BMP085.BMP085()
print "Content-type: text/html\n\n"
print '<h1>Temp = {0:0.2f} *C</h1>'.format(sensor.read_temperature())
print '<h1>Pressure = {0:0.2f} hPa</h1>'.format(sensor.read_pressure()/100)
result=get_data()
#print '<h1> Last database entry: </h1>'
#print '<h1>'
#print str(result)
##print '</h1>'

