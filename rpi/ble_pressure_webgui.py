#!/usr/bin/env python

"""
Not to readers:
I am not original creator of this file (not really into web stuff).
I used as basis for my purposes work from raspberrypiwebserver.com
Unfortunately I cannot find the site now, perhaps it was deleted. 
"""

import sqlite3
import sys
import cgi
import cgitb


# global variables
speriod=(15*60)-1
dbname='/var/www/ble_templog.db'



# print the HTTP header
def printHTTPheader():
    print "Content-type: text/html\n\n"



# print the HTML head section
# arguments are the page title and the table for the chart
def printHTMLHead(title, table):
    print "<head>"
    print "    <title>"
    print title
    print "    </title>"
    
    print_graph_script(table)

    print "</head>"


# get data from the database
# if an interval is passed, 
# return a list of records from the database
def get_data(interval):

    conn=sqlite3.connect(dbname)
    curs=conn.cursor()

    if interval == None:
        curs.execute("SELECT * FROM pressure")
    else:
        curs.execute("SELECT * FROM pressure WHERE timestamp>datetime('now','-%s hours')" % interval)

    rows=curs.fetchall()

    conn.close()

    return rows


# convert rows from database into a javascript table
def create_table(rows):
    chart_table=""

    for row in rows[:-1]:
        rowstr="['{0}', {1}],\n".format(str(row[0]),str(row[1]))
        chart_table+=rowstr

    row=rows[-1]
    rowstr="['{0}', {1}]\n".format(str(row[0]),str(row[1]))
    chart_table+=rowstr

    return chart_table


# print the javascript to generate the chart
# pass the table generated from the database info
def print_graph_script(table):

    # google chart snippet
    chart_code="""
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      google.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
          ['Time', 'Pressure'],
%s
        ]);
        var options = {
          title: 'Pressure'
        };
        var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
        chart.draw(data, options);
      }
    </script>"""

    print chart_code % (table)




# print the div that contains the graph
def show_graph():
    print "<h2>Pressure Chart</h2>"
    print '<div id="chart_div" style="width: 900px; height: 500px;"></div>'



# connect to the db and show some stats
# argument option is the number of hours
def show_stats(option):

    conn=sqlite3.connect(dbname)
    curs=conn.cursor()

    if option is None:
        option = str(24)

    curs.execute("SELECT timestamp,max(pressure) FROM pressure WHERE timestamp>datetime('now','-%s hour') AND timestamp<=datetime('now')" % option)
    rowmax=curs.fetchone()
    rowstrmax="{0}&nbsp&nbsp&nbsp{1}hPa".format(str(rowmax[0]),str(rowmax[1]))

    curs.execute("SELECT timestamp,min(pressure) FROM pressure WHERE timestamp>datetime('now','-%s hour') AND timestamp<=datetime('now')" % option)
    rowmin=curs.fetchone()
    rowstrmin="{0}&nbsp&nbsp&nbsp{1}hPa".format(str(rowmin[0]),str(rowmin[1]))

    curs.execute("SELECT avg(pressure) FROM pressure WHERE timestamp>datetime('now','-%s hour') AND timestamp<=datetime('now')" % option)
    rowavg=curs.fetchone()


    print "<hr>"


    print "<h2>Minumum pressure&nbsp</h2>"
    print rowstrmin
    print "<h2>Maximum pressure</h2>"
    print rowstrmax
    print "<h2>Average pressure</h2>"
    print "%.3f" % rowavg+"hPa"

    print "<hr>"

    print "<h2>In the last hour:</h2>"
    print "<table>"
    print "<tr><td><strong>Date/Time</strong></td><td><strong>Pressure</strong></td></tr>"

    rows=curs.execute("SELECT * FROM pressure WHERE timestamp>datetime('new','-1 hour') AND timestamp<=datetime('new')")
    for row in rows:
        rowstr="<tr><td>{0}&emsp;&emsp;</td><td>{1}hPa</td></tr>".format(str(row[0]),str(row[1]))
        print rowstr
    print "</table>"

    print "<hr>"

    conn.close()




def print_time_selector(option):

    print """<form action="/cgi-bin/ble_pressure_webgui.py" method="POST">
        Show the Pressure logs for  
        <select name="timeinterval">"""


    if option is not None:

        if option == "6":
            print "<option value=\"6\" selected=\"selected\">the last 6 hours</option>"
        else:
            print "<option value=\"6\">the last 6 hours</option>"

        if option == "12":
            print "<option value=\"12\" selected=\"selected\">the last 12 hours</option>"
        else:
            print "<option value=\"12\">the last 12 hours</option>"

        if option == "24":
            print "<option value=\"24\" selected=\"selected\">the last 24 hours</option>"
        else:
            print "<option value=\"24\">the last 24 hours</option>"

        if option == "72":
            print "<option value=\"72\">the last 72 hours</option>"
        else:
            print "<option value=\"72\">the last 72 hours</option>"

    else:
        print """<option value="6">the last 6 hours</option>
            <option value="12">the last 12 hours</option>
            <option value="24" selected="selected">the last 24 hours</option>
            <option value="12">the last 12 hours</option>"""

    print """        </select>
        <input type="submit" value="Display">
    </form>"""


# check that the option is valid
# and not an SQL injection
def validate_input(option_str):
    # check that the option string represents a number
    if option_str.isalnum():
        # check that the option is within a specific range
        if int(option_str) > 0 and int(option_str) <= 72:
            return option_str
        else:
            return None
    else: 
        return None


#return the option passed to the script
def get_option():
    form=cgi.FieldStorage()
    if "timeinterval" in form:
        option = form["timeinterval"].value
        return validate_input (option)
    else:
        return None




# main function
# This is where the program starts 
def main():

    cgitb.enable()

    # get options that may have been passed to this script
    option=get_option()

    if option is None:
        option = str(24)

    # get data from the database
    records=get_data(option)

    # print the HTTP header
    printHTTPheader()

    if len(records) != 0:
        # convert the data into a table
        table=create_table(records)
    else:
        print "No data found"
        return

    # start printing the page
    print "<html>"
    # print the head section including the table
    # used by the javascript for the chart
    printHTMLHead("BLE Pressure Logger", table)

    # print the page body
    print "<body>"
    print "<h1>BLE Pressure Logger</h1>"
    print "<hr>"
    print_time_selector(option)
    show_graph()
    show_stats(option)
    print "</body>"
    print "</html>"

    sys.stdout.flush()

if __name__=="__main__":
    main()

