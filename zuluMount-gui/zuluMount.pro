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
    ../zuluCrypt-gui/dialogmsg.cpp \
    ../zuluCrypt-gui/userfont.cpp \
    ../zuluCrypt-gui/tablewidget.cpp \
    mountpartition.cpp \
    ../zuluCrypt-gui/socketsendkey.cpp \
    oneinstance.cpp \
    ../zuluCrypt-gui/utility.cpp \
    auto_mount.cpp \
    auto_mount_helper.cpp \
    monitor_mountinfo.cpp \
    task.cpp \
    deviceoffset.cpp

HEADERS  += mainwindow.h\
    keydialog.h \
    ../zuluCrypt-gui/dialogmsg.h\
    ../zuluCrypt-gui/userfont.h \
    ../zuluCrypt-gui/tablewidget.h \
    mountpartition.h \
    ../zuluCrypt-gui/socketsendkey.h \
    oneinstance.h \
    ../zuluCrypt-gui/utility.h \
    auto_mount.h \
    auto_mount_helper.h \
    monitor_mountinfo.h \
    task.h \
    deviceoffset.h

TRANSLATIONS = ../translations/zuluMount/messages.ts

FORMS    += mainwindow.ui\
	    ../zuluCrypt-gui/dialogmsg.ui \
	    keydialog.ui \
    mountpartition.ui \
    deviceoffset.ui

RESOURCES += \
    icon.qrc

INCLUDEPATH +=/home/ink/projects/build/zuluCrypt /home/local/KDE4/include /home/local/QT/include

LIBS += -lzuluCryptPluginManager -lQtNetwork -lkwalletbackend -L/home/local/KDE4/lib -llxqtwallet
