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

 Read also the readme_usbtcm_driver.txt file.



