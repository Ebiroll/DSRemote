
This applies to the DS6000 series.

In order to let the system load the usbtmc driver automatically,
proceed as follows:

Create a new group, usbtmc:

sudo groupadd usbtmc

Add yourself to this group:

sudo usermod -a -G usbtmc <username>


Create a file /etc/udev/rules.d/usbtmc.rules with the following content:

####################################################################################
# USBTMC instruments

# Rigol Technologies  DS6000 series
SUBSYSTEMS=="usb", ACTION=="add", ATTRS{idVendor}=="1ab1", ATTRS{idProduct}=="04b0", GROUP="usbtmc", MODE="0660"

# Devices
KERNEL=="usbtmc/*",       MODE="0660", GROUP="usbtmc"
KERNEL=="usbtmc[0-9]*",   MODE="0660", GROUP="usbtmc"
####################################################################################


Disconnect and connect the oscilloscope again (or reboot).


