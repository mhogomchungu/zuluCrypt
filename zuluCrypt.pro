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
    createpartition.cpp

HEADERS  += zulucrypt.h password_dialog.h \
    openpartition.h \
    version.h \
    luksaddkey.h \
    luksdeletekey.h \
    createpartition.h

FORMS    += zulucrypt.ui password.ui \
    openartition.ui \
    luksaddkey.ui \
    luksdeletekey.ui \
    createpartition.ui

OTHER_FILES +=

























