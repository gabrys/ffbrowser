CONFIG += qt warn_on

win32 : Debug : CONFIG += console

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += webkit network

HEADERS += \
    browserdefaultsettings.h \
    browsersettings.h \
    browserwebpage.h \
    browserwidget.h \
    browserwindow.h \
    hotpages.h \
    jsproxy.h \
    menuwidget.h \
    styleupdater.h

SOURCES += \
    browserdefaultsettings.cpp \
    browsersettings.cpp \
    browserwebpage.cpp \
    browserwidget.cpp \
    browserwindow.cpp \
    hotpages.cpp \
    jsproxy.cpp \
    menuwidget.cpp \
    styleupdater.cpp

RESOURCES += \
    $$PWD/res/res.qrc
