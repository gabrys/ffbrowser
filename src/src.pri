CONFIG += qt warn_on

win32 : Debug : CONFIG += console

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += webkit network

HEADERS += \
    browsersettings.h \
    browserwidget.h \
    browserwindow.h \
    hotpages.h \
    jsproxy.h \
    menuwidget.h \
    styleupdater.h

SOURCES += \
    browsersettings.cpp \
    browserwidget.cpp \
    browserwindow.cpp \
    hotpages.cpp \
    jsproxy.cpp \
    menuwidget.cpp \
    styleupdater.cpp

RESOURCES += \
    $$PWD/res/res.qrc
