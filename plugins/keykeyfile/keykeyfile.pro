#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T19:48:55
#
#-------------------------------------------------

QT       += core gui

TARGET = keykeyfile
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
    ../kwallet/zulusocket.cpp

HEADERS  += mainwindow.h \
    ../kwallet/zulusocket.h

FORMS    += mainwindow.ui

LIBS += -lQtNetwork
