#-------------------------------------------------
#
# Project created by QtCreator 2012-08-03T05:17:03
#
#-------------------------------------------------

QT       += core gui

TARGET = zuluMount-gui
TEMPLATE = app


SOURCES += main.cpp\
	zulumount.cpp\
    keydialog.cpp \
    ../zuluCrypt-gui/dialogmsg.cpp \
    ../zuluCrypt-gui/userfont.cpp \
    ../zuluCrypt-gui/tablewidget.cpp \
    mountpartition.cpp \
    ../zuluCrypt-gui/socketsendkey.cpp \
    oneinstance.cpp \
    ../zuluCrypt-gui/utility.cpp \
    events.cpp \
    monitor_mountinfo.cpp \
    task.cpp \
    deviceoffset.cpp \
    mountoptions.cpp \
    volumepropery.cpp \
    zulumounttask.cpp \
    veracryptpimdialog.cpp

HEADERS  += zulumount.h\
    keydialog.h \
    ../zuluCrypt-gui/dialogmsg.h\
    ../zuluCrypt-gui/userfont.h \
    ../zuluCrypt-gui/tablewidget.h \
    mountpartition.h \
    ../zuluCrypt-gui/socketsendkey.h \
    oneinstance.h \
    ../zuluCrypt-gui/utility.h \
    events.h \
    monitor_mountinfo.h \
    task.h \
    deviceoffset.h \
    mountoptions.h \
    volumepropery.h \
    zulumounttask.h \
    veracryptpimdialog.h

TRANSLATIONS = ../translations/zuluMount/en_US.ts \
               ../translations/zuluMount/fr_FR.ts \
               ../translations/zuluMount/de_DE.ts

FORMS    += zulumount.ui\
	    ../zuluCrypt-gui/dialogmsg.ui \
	    keydialog.ui \
    mountpartition.ui \
    deviceoffset.ui \
    mountoptions.ui \
    veracryptpimdialog.ui

RESOURCES += \
    icon.qrc

INCLUDEPATH +=/home/ink/projects/build/zuluCrypt /home/local/KDE4/include /home/local/QT/include

LIBS += -lzuluCryptPluginManager -lQtNetwork -lkwalletbackend -L/home/local/KDE4/lib -llxqtwallet
