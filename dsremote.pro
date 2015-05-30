

TEMPLATE = app
TARGET = dsremote
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qt
CONFIG += warn_on
CONFIG += release
CONFIG += static
CONFIG += largefile

QT += widgets

QMAKE_CXXFLAGS += -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits -Wfatal-errors

OBJECTS_DIR = ./objects
MOC_DIR = ./moc

HEADERS += global.h
HEADERS += mainwindow.h
HEADERS += about_dialog.h
HEADERS += utils.h
HEADERS += tmc_dev.h
HEADERS += tled.h
HEADERS += edflib.h
HEADERS += signalcurve.h

SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += timer_handlers.cpp
SOURCES += save_data.cpp
SOURCES += interface.cpp
SOURCES += about_dialog.cpp
SOURCES += utils.c
SOURCES += tmc_dev.c
SOURCES += tled.cpp
SOURCES += edflib.c
SOURCES += signalcurve.cpp

RESOURCES = images.qrc

target.path = /usr/bin
target.files = dsremote
INSTALLS += target

icon_a.path = /usr/share/icons
icon_a.files = images/r.png
INSTALLS += icon_a

icon_b.path += /usr/share/pixmaps
icon_b.files = images/r.png
INSTALLS += icon_b

icon_c.path += /usr/share/icons/hicolor/48x48/apps
icon_c.files = images/r.png
INSTALLS += icon_c

icon_d.path += /usr/share/icons/hicolor/48x48/mimetypes
icon_d.files = images/r.png
INSTALLS += icon_d

desktop_link.path += /usr/share/applications
desktop_link.files += install/dsremote.desktop
INSTALLS += desktop_link




