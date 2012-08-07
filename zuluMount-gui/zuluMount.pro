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
    keydialog.cpp \
    managepartitionthread.cpp\
    ../zuluCrypt-gui/dialogmsg.cpp

HEADERS  += mainwindow.h\
    keydialog.h \
    ../zuluCrypt-gui/dialogmsg.h\
    managepartitionthread.h

FORMS    += mainwindow.ui\	    
	    ../zuluCrypt-gui/dialogmsg.ui \
	    keydialog.ui

RESOURCES += \
    icon.qrc

INCLUDEPATH +=/home/ink/build