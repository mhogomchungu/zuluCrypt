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
    openvolume.cpp \
    luksaddkey.cpp \
    luksdeletekey.cpp \
    createvolume.cpp \
    createfile.cpp \
    createkeyfile.cpp \
    utility.cpp \
    managedevicenames.cpp \
    partitionproperties.cpp \
    cryptoinfo.cpp \
    erasedevice.cpp \
    manageluksheader.cpp \
    cryptfiles.cpp \
    dialogmsg.cpp \
    managesystemvolumes.cpp \
    keystrength.cpp \
    userfont.cpp \
    tablewidget.cpp \
    socketsendkey.cpp \
    openvolumereadonly.cpp \
    ../zuluMount-gui/oneinstance.cpp \
    openmountpointinfilemanager.cpp \
    savemountpointpath.cpp \
    md5/md5.c \
    lxqt_wallet/backend/lxqtwallet.c \
    lxqt_wallet/frontend/secret_service.c \
    lxqt_wallet/frontend/password_dialog.cpp \
    lxqt_wallet/frontend/task.cpp \
    lxqt_wallet/frontend/lxqt_wallet.cpp \
    lxqt_wallet/frontend/lxqt_secret_service.cpp \
    lxqt_wallet/frontend/lxqt_kwallet.cpp \
    lxqt_wallet/frontend/lxqt_internal_wallet.cpp \
    lxqt_wallet/frontend/changepassworddialog.cpp \
    task.cpp \
    keyfiletask.cpp \
    filetask.cpp \
    erasetask.cpp \
    crypttask.cpp \
    walletconfig.cpp \
    walletconfiginput.cpp \
    createvolumedialog.cpp

HEADERS  += zulucrypt.h \
    password_dialog.h \
    openvolume.h \
    luksaddkey.h \
    luksdeletekey.h \
    createvolume.h \
    createfile.h \
    createkeyfile.h \
    utility.h \
    managedevicenames.h \
    partitionproperties.h \
    cryptoinfo.h \
    erasedevice.h \
    manageluksheader.h \
    cryptfiles.h \
    createvolumedialog.h \
    dialogmsg.h \
    managesystemvolumes.h \
    keystrength.h \
    userfont.h \
    tablewidget.h \
    socketsendkey.h \
    openvolumereadonly.h \
    ../zuluMount-gui/oneinstance.h \
    openmountpointinfilemanager.h \
    savemountpointpath.h \
    lxqt_wallet/backend/lxqtwallet.h \
    lxqt_wallet/frontend/password_dialog.h \
    lxqt_wallet/frontend/task.h \
    lxqt_wallet/frontend/lxqt_wallet.h \
    lxqt_wallet/frontend/lxqt_secret_service.h \
    lxqt_wallet/frontend/lxqt_kwallet.h \
    lxqt_wallet/frontend/lxqt_internal_wallet.h \
    lxqt_wallet/frontend/changepassworddialog.h \
    task.h \
    filetask.h \
    keyfiletask.h \
    erasetask.h \
    crypttask.h \
    walletconfig.h \
    walletconfiginput.h


FORMS    += zulucrypt.ui password.ui \
    openvolume.ui \
    luksaddkey.ui \
    luksdeletekey.ui \
    createvolume.ui \
    createfile.ui \
    createkeyfile.ui \
    managedevicenames.ui \
    cryptoinfo.ui \
    erasedevice.ui \
    manageluksheader.ui \
    cryptfiles.ui \
    createvolumedialog.ui \
    dialogmsg.ui \
    managesystemvolumes.ui \
    lxqt_wallet/frontend/password_dialog.ui \
    lxqt_wallet/frontend/changepassworddialog.ui \
    walletconfig.ui \
    walletconfiginput.ui

TRANSLATIONS = ../translations/zuluCrypt/messages.ts

RESOURCES = icon.qrc

LIBS += -lblkid -lpwquality -L/home/local/KDE4/lib/ -lkwalletbackend -lgcrypt -lzuluCryptPluginManager

QMAKE_CXXFLAGS += -D_FILE_OFFSET_BITS=64 -Wall

OTHER_FILES += \
    backupluksheaderui \
    lxqt_wallet/SOURCE \
    lxqt_wallet/README.md \
    lxqt_wallet/lxqt_wallet.pro.user \
   # lxqt_wallet/lxqt_wallet.pro \
    lxqt_wallet/LICENSE \
    lxqt_wallet/CMakeLists.txt \
    lxqt_wallet/changelog \
    lxqt_wallet/BUILD_INSTRUCTIONS \
    lxqt_wallet/backend/README \
    lxqt_wallet/backend/CMakeLists.txt \
    lxqt_wallet/frontend/README \
    lxqt_wallet/frontend/CMakeLists.txt \
    lxqt_wallet/frontend/secret_service/CMakeLists.txt

INCLUDEPATH +=/home/ink/build /home/local/KDE4/include /usr/include /usr/include/glib-2.0/ /usr/lib/glib-2.0/include/ /usr/include/libsecret-1 /usr/include/libsecret-1/libsecret
