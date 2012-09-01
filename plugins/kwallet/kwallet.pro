#-------------------------------------------------
#
# Project created by QtCreator 2012-08-10T23:43:26
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = kwallet
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    wallet.cpp \
    zulusocket.cpp \
    socketwritedata.cpp

QMAKE_CXXFLAGS += -I/home/local/KDE4/include -Wall

LIBS += -lkwalletbackend -L/home/local/KDE4/lib -lzuluCryptPluginManager


HEADERS += \
    wallet.h \
    zulusocket.h \
    socketwritedata.h
