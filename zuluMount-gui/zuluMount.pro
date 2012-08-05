#-------------------------------------------------
#
# Project created by QtCreator 2012-08-03T05:17:03
#
#-------------------------------------------------

QT       += core gui

TARGET = inkmount
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        dialogmsg.cpp \
    keydialog.cpp \
    managepartitionthread.cpp

HEADERS  += mainwindow.h\
	    dialogmsg.h \
    keydialog.h \
    managepartitionthread.h

FORMS    += mainwindow.ui\	    
	    dialogmsg.ui \
    keydialog.ui

RESOURCES += \
    icon.qrc
