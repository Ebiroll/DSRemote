# DSRemote
Operate your Rigol oscilloscope from your Linux desktop.
========================================================

How to compile:
---------------


- Install Git, make, the GCC compiler and the Qt4 development package:<br />
 Linux Mint 18: sudo apt-get install git-core make libqt4-dev libqt4-core g++<br />
 openSuse Leap 42.3: sudo zypper in git-core libqt4-devel<br />
 Fedora 25: sudo dnf install git-core qt-devel gcc-c++<br />
 Ubuntu 18.04: sudo apt-get install g++ make git-core qtbase5-dev-tools qtbase5-dev qt5-default<br />

- Download the source code:<br />
 git clone https://gitlab.com/Teuniz/DSRemote.git<br />
 cd DSRemote<br />

- Create the makefile:<br />
 Linux Mint 18: qmake<br />
 openSuse Leap 42.3: qmake<br />
 Fedora 25: qmake-qt4<br />
 Ubuntu 18.04: qmake<br />

- Compile the source code and install it:<br />
 make -j16<br />
 sudo make install<br />

Now you can run the program by typing: dsremote

 Read also the file readme_usbtcm_driver.txt
 and the file notes.txt.

Supported devices
---------------

DS6000 and DS1000Z series oscilloscopes.

### MSO4000/DS4000 series

There is some basic support like capturing screenshots.

Not tested yet:

* Record & Playback function
* Wave Inspector
* serial decoding.


DS1000Z series:
---------------
DSRemote assumes that, on your DS1054Z, all options are installed.
If that is not the case it's possible that the software does not work correctly.
On your scope, go to the menu Utility -> Options -> Installed and make sure you have the following options installed:

* DECODER RS232,I2C,SPI Official
* MEM_DEPTH 24M/12M/6M Official
* TRIGGER Timeout,Runt,Window,Delay,Setup,NthEdge,RS232,I2C,SPI Official
* RECORDER Record/Re-play Official
* BANDWIDTH 100M Official



Qt framework:
-------------
In case you want to use Qt5, the recommended way is not to use the Qt5 libraries that comes with your distro.
Instead, download and compile the Qt5 source and use that to compile DSRemote.
Here's a step by step list with instructions:

First, fulfill the requirements for Qt:

http://doc.qt.io/qt-5/linux.html

Debian/Ubuntu: sudo apt-get install build-essential libgl1-mesa-dev libcups2-dev libx11-dev

Fedora: sudo dnf groupinstall "C Development Tools and Libraries"
        sudo dnf install mesa-libGL-devel cups-devel libx11-dev

openSUSE: sudo zypper install -t pattern devel_basis
          sudo zypper install xorg-x11-devel cups-devel freetype-devel fontconfig-devel libxkbcommon-devel libxkbcommon-x11-devel

#############################################################################################

 Compile a static version of the Qt5 libraries excluding all modules that are not needed.

 This will not mess with your system libraries. The new compiled libraries will be stored

 in a new and separate directory: /usr/local/Qt-5.12.3-static

 It will not interfere with other Qt programs.

#############################################################################################

mkdir Qt5-source

cd Qt5-source

wget http://ftp1.nluug.nl/languages/qt/official_releases/qt/5.12/5.12.3/single/qt-everywhere-src-5.12.3.tar.xz

here is a list of download mirrors: https://download.qt.io/static/mirrorlist/
The Qt source package you are going to need is: qt-everywhere-src-5.12.3.tar.xz

tar -xvf qt-everywhere-src-5.12.3.tar.xz

cd qt-everywhere-src-5.12.3

./configure -v -prefix /usr/local/Qt-5.12.3-static -release -opensource -confirm-license -c++std c++11 -static -accessibility -fontconfig -skip qtdeclarative -skip qtconnectivity -skip qtmultimedia -qt-zlib -no-mtdev -no-journald -qt-libpng -qt-libjpeg -system-freetype -qt-harfbuzz -no-openssl -no-libproxy -no-glib -nomake examples -nomake tests -no-compile-examples -cups -no-evdev -no-dbus -no-eglfs -qreal double -no-opengl -skip qtlocation -skip qtsensors -skip qtwayland -skip qtgamepad -skip qtserialbus -skip qt3d -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtspeech -skip qtwebengine

(takes about 2 minutes)

make -j6  (change option -j according to number of available cpu cores e.g -j4 or -j8)

(takes about 11 minutes)

sudo make install

Now go to the directory that contains the DSRemote sourcecode and enter the following commands:

/usr/local/Qt-5.12.3-static/bin/qmake

make -j6  (change option -j according to number of available cpu cores e.g -j4 or -j8)

sudo make install

Now you can run the program by typing: dsremote

Congratulations!
You have compiled a static version of DSRemote that can be deployed on other systems without the need
to install the Qt libraries.
In order to reduce the size of the executable, run the following commands:

strip -s dsremote

upx dsremote

