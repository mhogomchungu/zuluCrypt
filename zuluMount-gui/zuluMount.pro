#-------------------------------------------------
#
# Project created by QtCreator 2012-08-03T05:17:03
#
#-------------------------------------------------

QT       += core gui

TARGET = zuluMount-gui
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp\
    keydialog.cpp \
    managepartitionthread.cpp\
    ../zuluCrypt-gui/dialogmsg.cpp \
    ../zuluCrypt-gui/userfont.cpp \
    ../zuluCrypt-gui/tablewidget.cpp \
    mountpartition.cpp \
    ../zuluCrypt-gui/openvolumereadonly.cpp \
    ../zuluCrypt-gui/socketsendkey.cpp \
    oneinstance.cpp \
    ../zuluCrypt-gui/openmountpointinfilemanager.cpp \
    ../zuluCrypt-gui/savemountpointpath.cpp \
    ../zuluCrypt-gui/utility.cpp \
    auto_mount.cpp \
    auto_mount_helper.cpp

HEADERS  += mainwindow.h\
    keydialog.h \
    ../zuluCrypt-gui/dialogmsg.h\
    managepartitionthread.h \
    ../zuluCrypt-gui/userfont.h \
    ../zuluCrypt-gui/tablewidget.h \
    mountpartition.h \
    ../zuluCrypt-gui/openvolumereadonly.h \
    ../zuluCrypt-gui/socketsendkey.h \
    oneinstance.h \
    ../zuluCrypt-gui/openmountpointinfilemanager.h \
    ../zuluCrypt-gui/savemountpointpath.h \
    ../zuluCrypt-gui/utility.h \
    auto_mount.h \
    auto_mount_helper.h

TRANSLATIONS = ../translations/zuluMount/en_US.ts

FORMS    += mainwindow.ui\
	    ../zuluCrypt-gui/dialogmsg.ui \
	    keydialog.ui \
    mountpartition.ui

RESOURCES += \
    icon.qrc

INCLUDEPATH +=/home/ink/build /home/local/KDE4/include

LIBS += -lzuluCryptPluginManager -lQtNetwork
