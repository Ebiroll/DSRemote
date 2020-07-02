  
contains(QT_MAJOR_VERSION, 4) {

LIST = 0 1 2 3 4 5 6
for(a, LIST):contains(QT_MINOR_VERSION, $$a):error("This project needs Qt4 version >= 4.7.1 or Qt5 version >= 5.9.1")

contains(QT_MINOR_VERSION, 7) {
  LIST = 0
  for(a, LIST):contains(QT_PATCH_VERSION, $$a):error("This project needs Qt4 version >= 4.7.1 or Qt5 version >= 5.9.1")
}
}


contains(QT_MAJOR_VERSION, 5) {

LIST = 0 1 2 3 4 5 6 7 8
for(a, LIST):contains(QT_MINOR_VERSION, $$a):error("This project needs Qt4 version >= 4.7.1 or Qt5 version >= 5.9.1")

contains(QT_MINOR_VERSION, 9) {
  LIST = 0
  for(a, LIST):contains(QT_PATCH_VERSION, $$a):error("This project needs Qt4 version >= 4.7.1 or Qt5 version >= 5.9.1")
}
}


TEMPLATE = app
TARGET = dsremote
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt debug
CONFIG += warn_on
#CONFIG += release
CONFIG += static
CONFIG += largefile

QT += widgets
QT += network

QMAKE_CXXFLAGS += -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits -Wfatal-errors

OBJECTS_DIR = ./objects
MOC_DIR = ./moc

HEADERS += global.h
HEADERS += mainwindow.h
HEADERS += about_dialog.h
HEADERS += utils.h
HEADERS += connection.h
HEADERS += tmc_dev.h
HEADERS += tmc_lan.h
HEADERS += tled.h
HEADERS += edflib.h
HEADERS += signalcurve.h
HEADERS += settings_dialog.h
HEADERS += screen_thread.h
HEADERS += lan_connect_thread.h
HEADERS += read_settings_thread.h
HEADERS += save_data_thread.h
HEADERS += decode_dialog.h
HEADERS += tdial.h
HEADERS += wave_dialog.h
HEADERS += wave_view.h
HEADERS += playback_dialog.h

HEADERS += third_party/kiss_fft/kiss_fft.h
HEADERS += third_party/kiss_fft/_kiss_fft_guts.h
HEADERS += third_party/kiss_fft/kiss_fftr.h

SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += mainwindow_constr.cpp
SOURCES += timer_handlers.cpp
SOURCES += save_data.cpp
SOURCES += interface.cpp
SOURCES += serial_decoder.cpp
SOURCES += about_dialog.cpp
SOURCES += utils.c
SOURCES += connection.cpp
SOURCES += tmc_dev.c
SOURCES += tmc_lan.c
SOURCES += tled.cpp
SOURCES += edflib.c
SOURCES += signalcurve.cpp
SOURCES += settings_dialog.cpp
SOURCES += screen_thread.cpp
SOURCES += lan_connect_thread.cpp
SOURCES += read_settings_thread.cpp
SOURCES += save_data_thread.cpp
SOURCES += decode_dialog.cpp
SOURCES += tdial.cpp
SOURCES += wave_dialog.cpp
SOURCES += wave_view.cpp
SOURCES += playback_dialog.cpp

SOURCES += third_party/kiss_fft/kiss_fft.c
SOURCES += third_party/kiss_fft/kiss_fftr.c

RESOURCES = images.qrc

target.path = /usr/bin
target.files = dsremote
INSTALLS += target

icon_a.path = /usr/share/icons
icon_a.files = images/r_dsremote.png
INSTALLS += icon_a

icon_b.path += /usr/share/pixmaps
icon_b.files = images/r_dsremote.png
INSTALLS += icon_b

icon_c.path += /usr/share/icons/hicolor/48x48/apps
icon_c.files = images/r_dsremote.png
INSTALLS += icon_c

icon_d.path += /usr/share/icons/hicolor/48x48/mimetypes
icon_d.files = images/r_dsremote.png
INSTALLS += icon_d

desktop_link.path += /usr/share/applications
desktop_link.files += install/dsremote.desktop
INSTALLS += desktop_link

udev_rule.path += /etc/udev/rules.d
udev_rule.files += install/30-usbtmc.rules
INSTALLS += udev_rule


