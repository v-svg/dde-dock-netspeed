QT              += widgets svg gui
TEMPLATE         = lib
CONFIG          += c++11 plugin link_pkgconfig
PKGCONFIG       += dtkwidget

TARGET           = netspeed
DISTFILES       += netspeed.json

HEADERS += \
    monitorwidget.h \
    netspeedplugin.h \
    netspeedwidget.h \
    network_monitor.h \
    smooth_curve_generator.h

SOURCES += \
    monitorwidget.cpp \
    netspeedplugin.cpp \
    netspeedwidget.cpp \
    network_monitor.cpp \
    smooth_curve_generator.cpp