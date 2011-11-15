#-------------------------------------------------
#
# Project created by QtCreator 2011-09-09T10:15:19
#
#-------------------------------------------------

QT       += core gui

TARGET = zuluCrypt
TEMPLATE = app


SOURCES += main.cpp\
        zulucrypt.cpp\
        password_dialog.cpp \
    openpartition.cpp \
    luksaddkey.cpp \
    luksdeletekey.cpp \
    createpartition.cpp \
    createfile.cpp \
    createkeyfile.cpp \
    zulucryptthreads.cpp \
    startupupdateopenedvolumes.cpp \
    closeallvolumesthread.cpp

HEADERS  += zulucrypt.h password_dialog.h \
    openpartition.h \
    luksaddkey.h \
    luksdeletekey.h \
    createpartition.h \
    createfile.h \
    createkeyfile.h \
    zulucryptthreads.h \
    startupupdateopenedvolumes.h \
    closeallvolumesthread.h

FORMS    += zulucrypt.ui password.ui \
    openpartition.ui \
    luksaddkey.ui \
    luksdeletekey.ui \
    createpartition.ui \
    createfile.ui \
    createkeyfile.ui \
    rngselector.ui

TRANSLATIONS = ../translations/swahili_la.ts

RESOURCES = icon.qrc








