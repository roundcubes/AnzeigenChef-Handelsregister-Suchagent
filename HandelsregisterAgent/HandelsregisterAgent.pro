QT       += core network

TARGET = HandelsregisterAgent
TEMPLATE = lib
CONFIG += plugin

macx{
    DESTDIR = $$PWD/../build/release/macos
}

win32{
    DESTDIR = $$PWD\\..\\build\\release\\windows
}

linux{
    DESTDIR = $$PWD/../build/release/linux
}


DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        handelsregisteragentplugin.cpp

HEADERS += \
        handelsregisteragentplugin.h \
        searchagentinterface.h

DISTFILES += HandelsregisterAgent.json
