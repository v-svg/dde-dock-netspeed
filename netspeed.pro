QT              += widgets svg gui
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dtkwidget

TARGET           = netspeed
DESTDIR          = $$_PRO_FILE_PWD_
DISTFILES       += netspeed.json

HEADERS += \
    floatingwidget.h \
    monitorwidget.h \
    netspeedplugin.h \
    netspeeditem.h \
    network_monitor.h \
    smooth_curve_generator.h

SOURCES += \
    floatingwidget.cpp \
    monitorwidget.cpp \
    netspeedplugin.cpp \
    netspeeditem.cpp \
    network_monitor.cpp \
    smooth_curve_generator.cpp

RESOURCES += images.qrc
