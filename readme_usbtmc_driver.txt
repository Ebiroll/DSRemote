

The DS1054Z is plug and play with a recent kernelversion.
You probably have to add yourself to the group "usbtmc" before you can access the device.
Here's how you do that:
First, check the groupname of the device, enter:

ls -l /dev/usbtmc*

Now, add yourself to the group (usbtmc in this example):

sudo usermod -a -G usbtmc <username>
You need to logout and login again to let the changes take into effect.

In case you have a permission problem because /dev/usbtmc0 uid and gid are set to root,
create a file /lib/udev/rules.d/30-usbtmc.rules that contains the following:

####################################################################################
# USBTMC instruments

# Rigol Technologies  DS1000Z series
SUBSYSTEMS=="usb", ACTION=="add", ATTRS{idVendor}=="1ab1", ATTRS{idProduct}=="04ce", GROUP="usbtmc", MODE="0660"

# Devices
KERNEL=="usbtmc/*",       MODE="0660", GROUP="usbtmc"
KERNEL=="usbtmc[0-9]*",   MODE="0660", GROUP="usbtmc"
####################################################################################

You need to reboot in order to take it into effect.


///////////////////////////////////////////////////////////////////////////////

The following applies to the DS6000 series.

In order to let the system load the usbtmc driver automatically,
proceed as follows:

Create a new group, usbtmc:

sudo groupadd usbtmc

Add yourself to this group:

sudo usermod -a -G usbtmc <username>


Create a file /lib/udev/rules.d/30-usbtmc.rules with the following content:

####################################################################################
# USBTMC instruments

# Rigol Technologies  DS6000 series
SUBSYSTEMS=="usb", ACTION=="add", ATTRS{idVendor}=="1ab1", ATTRS{idProduct}=="04b0", GROUP="usbtmc", MODE="0660"

# Devices
KERNEL=="usbtmc/*",       MODE="0660", GROUP="usbtmc"
KERNEL=="usbtmc[0-9]*",   MODE="0660", GROUP="usbtmc"
####################################################################################


You need to reboot in order to take it into effect.


