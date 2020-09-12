#!/bin/bash

set -x

# copy relevant files
cp ble_pressure_webgui.py /usr/lib/cgi-bin/ble_pressure_webgui.py
cp ble_humidity_webgui.py /usr/lib/cgi-bin/ble_humidity_webgui.py
cp ble_temp_webgui.py /usr/lib/cgi-bin/ble_temp_webgui.py
cp index.php /var/www/html/index.php 

# add permissions
chmod 755 /usr/lib/cgi-bin/ble_pressure_webgui.py
chmod 755 /usr/lib/cgi-bin/ble_humidity_webgui.py
chmod 755 /usr/lib/cgi-bin/ble_temp_webgui.py

chown www-data /usr/lib/cgi-bin/ble_pressure_webgui.py
chown www-data /usr/lib/cgi-bin/ble_humidity_webgui.py
chown www-data /usr/lib/cgi-bin/ble_temp_webgui.py