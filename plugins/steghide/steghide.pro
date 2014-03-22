#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T19:48:55
#
#-------------------------------------------------

QT       += core gui

TARGET = gpg
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
    ../../zuluCrypt-gui/dialogmsg.cpp \
    ../../zuluCrypt-gui/socketsendkey.cpp \
    getgpgkey.cpp

HEADERS  += mainwindow.h \
    ../../zuluCrypt-gui/dialogmsg.h \
    ../../zuluCrypt-gui/socketsendkey.h \
    getgpgkey.h

FORMS    += mainwindow.ui ../../zuluCrypt-gui/dialogmsg.ui

LIBS += -lzuluCryptPluginManager
