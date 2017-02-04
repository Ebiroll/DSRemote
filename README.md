# DSRemote
Operate your Rigol oscilloscope from your Linux desktop.
========================================================

How to compile:
---------------


- Install Git, the GCC compiler and the Qt4 development package:<br />
 Linux Mint 18: sudo apt-get install git-core libqt4-dev libqt4-core g++<br />
 openSuse Leap 42.2: sudo zypper in git-core libqt4-devel<br />
 Fedora 25: sudo dnf install git-core qt-devel gcc-c++<br />

- Download the source code:<br />
 mkdir DSRemote_git<br />
 cd DSRemote_git<br />
 git init<br />
 git pull https://github.com/Teuniz/DSRemote<br />

- Create the makefile:<br />
 Linux Mint 18: qmake<br />
 openSuse Leap 42.2: qmake<br />
 Fedora 25: qmake-qt4<br />

- Compile the source code and install it:<br />
 make -j16<br />
 sudo make install<br />

Now you can run the program by typing: dsremote

 Read also the file readme_usbtcm_driver.txt
 and the file notes.txt.



