CONFIG += qt warn_on

win32 : Debug : CONFIG += console

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += webkit network

HEADERS += \
    browserwidget.h \
    browserwindow.h \
    menuwidget.h \
    styleupdater.h

SOURCES += \
    browserwidget.cpp \
    browserwindow.cpp \
    menuwidget.cpp \
    styleupdater.cpp

RESOURCES += \
    $$PWD/res/res.qrc
