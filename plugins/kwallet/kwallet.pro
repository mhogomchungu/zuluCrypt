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
    ../../zuluCrypt-gui/socketsendkey.cpp \
    ../../zuluCrypt-gui/zuluoptions.cpp

QMAKE_CXXFLAGS += -I/home/local/KDE4/include -Wall

LIBS += -lkwalletbackend -L/home/local/KDE4/lib -lzuluCryptPluginManager

INCLUDEPATH +=/home/ink/build /home/local/KDE4/include

HEADERS += \
    wallet.h \
    ../../zuluCrypt-gui/socketsendkey.h \
    ../../zuluCrypt-gui/zuluoptions.h
