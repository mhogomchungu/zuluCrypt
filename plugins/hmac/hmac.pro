#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T19:48:55
#
#-------------------------------------------------

QT       += core gui

TARGET = hmac
TEMPLATE = app

SOURCES += main.cpp\
	../mainwindow.cpp \
    ../../zuluCrypt-gui/dialogmsg.cpp

HEADERS  += ../mainwindow.h \
    ../../zuluCrypt-gui/dialogmsg.h

FORMS    += ../mainwindow.ui  ../../zuluCrypt-gui/dialogmsg.ui

QMAKE_CXXFLAGS += -std=c++11
