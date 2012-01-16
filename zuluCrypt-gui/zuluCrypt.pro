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
    startupupdateopenedvolumes.cpp \
    closeallvolumesthread.cpp \
    runinthread.cpp \
    miscfunctions.cpp \
    managedevicenames.cpp \
    partitionproperties.cpp \
    volumepropertiesthread.cpp \
    createfilethread.cpp \
    closevolumethread.cpp

HEADERS  += zulucrypt.h password_dialog.h \
    openpartition.h \
    luksaddkey.h \
    luksdeletekey.h \
    createpartition.h \
    createfile.h \
    createkeyfile.h \
    startupupdateopenedvolumes.h \
    closeallvolumesthread.h \
    runinthread.h \
    miscfunctions.h \
    managedevicenames.h \
    partitionproperties.h \
    volumepropertiesthread.h \
    createfilethread.h \
    closevolumethread.h

FORMS    += zulucrypt.ui password.ui \
    openpartition.ui \
    luksaddkey.ui \
    luksdeletekey.ui \
    createpartition.ui \
    createfile.ui \
    createkeyfile.ui \
    managedevicenames.ui

TRANSLATIONS = 

RESOURCES = icon.qrc

LIBS += -lblkid

QMAKE_CXXFLAGS += -D_FILE_OFFSET_BITS=64 -Wall






