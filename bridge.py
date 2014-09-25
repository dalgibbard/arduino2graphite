#!/bin/env python

######################## MODULE IMPORTS
import sys
from time import time
try:
    from serial import Serial
except ImportError:
    print("This program requires pySerial. Install using:")
    print("sudo pip install pyserial")
    print("")
    sys.exit(1)
try:
    import graphitesend
except ImportError:
    print("This program requires graphitesend. Install using:")
    print("sudo pip install graphitesend")
    print("")
    sys.exit(1)
######################## END MODULE IMPORTS

######################## CONFIGURATION
# Bridge Settings
set_graphite_server = "dg-graphite1.cloudapp.net"
set_graphite_port = "2003"
set_system_name = None
set_prefix = None
set_suffix = None
set_connect_on_create = True
set_group = "arduino"
set_lowercase_metric_names = True
set_fqdn_squash = False
set_dryrun = True
set_debug = True

# Code Settings
### Allowed variance on the Realtime Clock - '5' allows for the clock
### to be 5s higher or lower than the system time before correcting.
###   Warning- setting this too low may cause the system to trigger time set constantly...
clock_variance=3
serial_device='/dev/ttyACM0'
baud_rate=9600
######################## END CONFIGURATION

######################## CODE START
# Setup Serial Connection
ser = Serial(serial_device, baud_rate)
# Gather data from Serial - strip return chars, and assign each word to the list 'x'
x = ser.readline().strip('\n\r').split()

# Status is list item '0'. Force to string.
status = str(x[0])
# List item '1' is the epoch timestamp reported from the Arduino RTC
epoch = int(x[1])
# List item '2' is the humidity value (0 - 100)
humid = x[2]
# List item '3' is the Temperature in Celsius
temp = x[3]

# Is the reported Status is not 'OK', print error and skip submission.
if not status == "OK":
    print("Sensor error occured: " + status)
# Else; status is OK, and we should submit to Graphite
else:
    maxtime=epoch + clock_variance
    mintime=epoch - clock_variance
    nowtime=int(time())
    # If the Arduino reported time varies from system time by 'clock_variance'...
    if nowtime < mintime or nowtime > maxtime:
        print("Time variance detected. Setting clock.")
        # Write T<EPOCH> to Serial for the Arduino to set it's clock.
        ser.write("T" + str(time()))
        # Re-map the epoch to the current system time, to avoid
        # sending out-of-sync metrics (and without skipping a submit!)
        epoch=time()
    
    # Create our submit handler
    g = graphitesend.init(prefix=set_prefix, graphite_server=set_graphite_server, graphite_port=set_graphite_port, debug=set_debug, group=set_group, system_name=set_system_name, suffix=set_suffix, lowercase_metric_names=set_lowercase_metric_names, connect_on_create=set_connect_on_create, fqdn_squash=set_fqdn_squash, dryrun=True)
    # Send our data
    print g.send_list([('temperature', temp), ('humidity', humid)], timestamp=epoch)
######################## CODE END