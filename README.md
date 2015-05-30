# DSRemote
Operate your Rigol oscilloscope from your Linux desktop.
========================================================

How to compile:
---------------

- Install the Qt4 development package.
 For example, on openSuse, start Yast and install the package "libqt4-devel".
 On Ubuntu, open a console and enter: sudo apt-get install libqt4-dev libqt4-core g++

- Now, open a terminal (console) and cd to the directory that contains the source files and
 enter the following commands:

 qmake

 make -j16

 sudo make install

 Now you can run the program by typing: dsremote


In order to let the system load the usbtmc driver automatically,
proceed as follows:

Create a new group, usbtmc:

sudo groupadd usbtmc

Add yourself to this group:

sudo usermod -a -G usbtmc <your username>


Create a file /etc/udev/rules.d/usbtmc.rules with the following content:

  ####################################################################################
  # USBTMC instruments

  # Rigol Technologies  DS6000 series
  SUBSYSTEMS=="usb", ACTION=="add", ATTRS{idVendor}=="1ab1", ATTRS{idProduct}=="04b0", GROUP="usbtmc", MODE="0660"

  # Devices
  KERNEL=="usbtmc/*",       MODE="0660", GROUP="usbtmc"
  KERNEL=="usbtmc[0-9]*",   MODE="0660", GROUP="usbtmc"
  ####################################################################################


Disconnect and connect the osciloscope again (or reboot).




