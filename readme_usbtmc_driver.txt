
>>>>>>>>>>>>>> Warning <<<<<<<<<<<<<<<<<<<<

The firmware of the DS1054Z has a bug in the USB driver.

The scope uses HS bulk endpoints with a packetsize of 64 bytes which is illegal because the standard says it must be
512 bytes. So, some controllers don't forward the packet to the driver (or the driver doesn't forward it to an upper layer)
because it expects more data to arrive.

In my experience and from what users of DSRemote reported to me, there's usually no problem
with older hardware (motherboard USB chipsets that supports USB 1 & 2 only).
In case your pc has a more recent/modern chipset, it's very likely that the scope will not work via USB!

I reported this bug long time ago (March 2017) to Rigol and their response was:

"the bug is confirmed, but it seems to hard to fix it.
A fix will be implemted in new Models, for older ones, no confirmation to
have a fix available soon."

If this bug occurs, use LAN connection instead (and contact Rigol to report it and ask for a firmware update).

>>>>>>>>>>>>>> End of warning <<<<<<<<<<<<<<<<<<<<<<

The DS1054Z is plug and play with a recent kernelversion.
You probably have to add yourself to the group "usbtmc" before you can access the device.
Here's how you do that:

First, check the groupname of the device, enter:

ls -l /dev/usbtmc*

In case it is root, enter the command:

sudo groupadd usbtmc

Now, add yourself to the group:

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


