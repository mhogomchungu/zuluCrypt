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
    ../kwallet/zulusocket.cpp \
    ../../zuluCrypt-gui/dialogmsg.cpp

HEADERS  += mainwindow.h \
    ../kwallet/zulusocket.h \
    ../../zuluCrypt-gui/dialogmsg.h

FORMS    += mainwindow.ui ../../zuluCrypt-gui/dialogmsg.ui

LIBS += -lQtNetwork
