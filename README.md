# DSRemote
Operate your Rigol oscilloscope from your Linux desktop.
========================================================

How to compile:
---------------


- Install Git, the GCC compiler and the Qt4 development package:
 Linux Mint 18: sudo apt-get install git-core libqt4-dev libqt4-core g++
 openSuse Leap 42.2: sudo zypper in git-core libqt4-devel
 Fedora 25: sudo dnf install git-core qt-devel gcc-c++

- Download the source code:
 mkdir DSRemote_git
 cd DSRemote_git
 git init
 git pull https://github.com/Teuniz/DSRemote

- Create the makefile:
 Linux Mint 18: qmake
 openSuse Leap 42.2: qmake
 Fedora 25: qmake-qt4

- Compile the source code and install it:
 make -j16
 sudo make install

Now you can run the program by typing: dsremote

 Read also the file readme_usbtcm_driver.txt
 and the file notes.txt.



