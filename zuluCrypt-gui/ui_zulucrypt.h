/********************************************************************************
** Form generated from reading UI file 'zulucrypt.ui'
**
** Created: Wed Dec 28 22:55:13 2011
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZULUCRYPT_H
#define UI_ZULUCRYPT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_zuluCrypt
{
public:
    QAction *actionFileOpen;
    QAction *actionPartitionOpen;
    QAction *actionFileCreate;
    QAction *actionPartitionCreate;
    QAction *actionAbout;
    QAction *actionAddKey;
    QAction *actionDeleteKey;
    QAction *actionInfo;
    QAction *actionCreatekeyFile;
    QAction *actionTray_icon;
    QAction *actionFonts;
    QAction *actionFavorite_volumes;
    QAction *actionManage_favorites;
    QAction *actionSelect_random_number_generator;
    QAction *actionClose_application;
    QAction *action_minimize;
    QAction *actionMinimize_to_tray;
    QAction *action_close;
    QAction *actionClose_all_opened_volumes;
    QAction *actionManage_names;
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QMenuBar *menuBar;
    QMenu *menuOpen;
    QMenu *menuCreate;
    QMenu *menuHelp;
    QMenu *menuConfigure;
    QMenu *menuOptions;
    QMenu *menuFavorites;
    QMenu *menu_zc;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *zuluCrypt)
    {
        if (zuluCrypt->objectName().isEmpty())
            zuluCrypt->setObjectName(QString::fromUtf8("zuluCrypt"));
        zuluCrypt->setWindowModality(Qt::WindowModal);
        zuluCrypt->setEnabled(true);
        zuluCrypt->resize(722, 419);
        QIcon icon;
        icon.addFile(QString::fromUtf8("zuluCrypt.png"), QSize(), QIcon::Normal, QIcon::Off);
        zuluCrypt->setWindowIcon(icon);
        actionFileOpen = new QAction(zuluCrypt);
        actionFileOpen->setObjectName(QString::fromUtf8("actionFileOpen"));
        actionPartitionOpen = new QAction(zuluCrypt);
        actionPartitionOpen->setObjectName(QString::fromUtf8("actionPartitionOpen"));
        actionFileCreate = new QAction(zuluCrypt);
        actionFileCreate->setObjectName(QString::fromUtf8("actionFileCreate"));
        actionFileCreate->setEnabled(true);
        actionFileCreate->setVisible(true);
        actionPartitionCreate = new QAction(zuluCrypt);
        actionPartitionCreate->setObjectName(QString::fromUtf8("actionPartitionCreate"));
        actionPartitionCreate->setEnabled(true);
        actionPartitionCreate->setVisible(true);
        actionAbout = new QAction(zuluCrypt);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAddKey = new QAction(zuluCrypt);
        actionAddKey->setObjectName(QString::fromUtf8("actionAddKey"));
        actionDeleteKey = new QAction(zuluCrypt);
        actionDeleteKey->setObjectName(QString::fromUtf8("actionDeleteKey"));
        actionInfo = new QAction(zuluCrypt);
        actionInfo->setObjectName(QString::fromUtf8("actionInfo"));
        actionCreatekeyFile = new QAction(zuluCrypt);
        actionCreatekeyFile->setObjectName(QString::fromUtf8("actionCreatekeyFile"));
        actionTray_icon = new QAction(zuluCrypt);
        actionTray_icon->setObjectName(QString::fromUtf8("actionTray_icon"));
        actionFonts = new QAction(zuluCrypt);
        actionFonts->setObjectName(QString::fromUtf8("actionFonts"));
        actionFavorite_volumes = new QAction(zuluCrypt);
        actionFavorite_volumes->setObjectName(QString::fromUtf8("actionFavorite_volumes"));
        actionManage_favorites = new QAction(zuluCrypt);
        actionManage_favorites->setObjectName(QString::fromUtf8("actionManage_favorites"));
        actionSelect_random_number_generator = new QAction(zuluCrypt);
        actionSelect_random_number_generator->setObjectName(QString::fromUtf8("actionSelect_random_number_generator"));
        actionClose_application = new QAction(zuluCrypt);
        actionClose_application->setObjectName(QString::fromUtf8("actionClose_application"));
        action_minimize = new QAction(zuluCrypt);
        action_minimize->setObjectName(QString::fromUtf8("action_minimize"));
        actionMinimize_to_tray = new QAction(zuluCrypt);
        actionMinimize_to_tray->setObjectName(QString::fromUtf8("actionMinimize_to_tray"));
        action_close = new QAction(zuluCrypt);
        action_close->setObjectName(QString::fromUtf8("action_close"));
        actionClose_all_opened_volumes = new QAction(zuluCrypt);
        actionClose_all_opened_volumes->setObjectName(QString::fromUtf8("actionClose_all_opened_volumes"));
        actionManage_names = new QAction(zuluCrypt);
        actionManage_names->setObjectName(QString::fromUtf8("actionManage_names"));
        centralWidget = new QWidget(zuluCrypt);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 0, 701, 381));
        tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableWidget->setFrameShadow(QFrame::Raised);
        tableWidget->setMidLineWidth(1);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setTabKeyNavigation(false);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
        tableWidget->setGridStyle(Qt::NoPen);
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(3);
        zuluCrypt->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(zuluCrypt);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 722, 22));
        menuOpen = new QMenu(menuBar);
        menuOpen->setObjectName(QString::fromUtf8("menuOpen"));
        menuCreate = new QMenu(menuBar);
        menuCreate->setObjectName(QString::fromUtf8("menuCreate"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuConfigure = new QMenu(menuBar);
        menuConfigure->setObjectName(QString::fromUtf8("menuConfigure"));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        menuFavorites = new QMenu(menuBar);
        menuFavorites->setObjectName(QString::fromUtf8("menuFavorites"));
        menu_zc = new QMenu(menuBar);
        menu_zc->setObjectName(QString::fromUtf8("menu_zc"));
        zuluCrypt->setMenuBar(menuBar);
        statusBar = new QStatusBar(zuluCrypt);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        zuluCrypt->setStatusBar(statusBar);

        menuBar->addAction(menu_zc->menuAction());
        menuBar->addAction(menuOpen->menuAction());
        menuBar->addAction(menuCreate->menuAction());
        menuBar->addAction(menuConfigure->menuAction());
        menuBar->addAction(menuFavorites->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuOpen->addAction(actionFileOpen);
        menuOpen->addAction(actionPartitionOpen);
        menuOpen->addSeparator();
        menuCreate->addAction(actionFileCreate);
        menuCreate->addAction(actionPartitionCreate);
        menuCreate->addAction(actionCreatekeyFile);
        menuHelp->addAction(actionInfo);
        menuHelp->addAction(actionAbout);
        menuConfigure->addAction(actionAddKey);
        menuConfigure->addAction(actionDeleteKey);
        menuOptions->addAction(actionTray_icon);
        menuOptions->addAction(actionFonts);
        menuOptions->addAction(actionManage_names);
        menu_zc->addAction(action_minimize);
        menu_zc->addAction(actionMinimize_to_tray);
        menu_zc->addSeparator();
        menu_zc->addAction(actionClose_all_opened_volumes);
        menu_zc->addSeparator();
        menu_zc->addAction(action_close);

        retranslateUi(zuluCrypt);

        QMetaObject::connectSlotsByName(zuluCrypt);
    } // setupUi

    void retranslateUi(QMainWindow *zuluCrypt)
    {
        zuluCrypt->setWindowTitle(QApplication::translate("zuluCrypt", "zuluCrypt", 0, QApplication::UnicodeUTF8));
        actionFileOpen->setText(QApplication::translate("zuluCrypt", "encrypted file", 0, QApplication::UnicodeUTF8));
        actionFileOpen->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
        actionPartitionOpen->setText(QApplication::translate("zuluCrypt", "encrypted partition", 0, QApplication::UnicodeUTF8));
        actionPartitionOpen->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        actionFileCreate->setText(QApplication::translate("zuluCrypt", "encrypted file", 0, QApplication::UnicodeUTF8));
        actionFileCreate->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+A", 0, QApplication::UnicodeUTF8));
        actionPartitionCreate->setText(QApplication::translate("zuluCrypt", "encrypted partition", 0, QApplication::UnicodeUTF8));
        actionPartitionCreate->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("zuluCrypt", "about", 0, QApplication::UnicodeUTF8));
        actionAbout->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+R", 0, QApplication::UnicodeUTF8));
        actionAddKey->setText(QApplication::translate("zuluCrypt", "add key", 0, QApplication::UnicodeUTF8));
        actionAddKey->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+U", 0, QApplication::UnicodeUTF8));
        actionDeleteKey->setText(QApplication::translate("zuluCrypt", "delete key", 0, QApplication::UnicodeUTF8));
        actionDeleteKey->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+W", 0, QApplication::UnicodeUTF8));
        actionInfo->setText(QApplication::translate("zuluCrypt", "crypto info", 0, QApplication::UnicodeUTF8));
        actionInfo->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+E", 0, QApplication::UnicodeUTF8));
        actionCreatekeyFile->setText(QApplication::translate("zuluCrypt", "key file", 0, QApplication::UnicodeUTF8));
        actionCreatekeyFile->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+D", 0, QApplication::UnicodeUTF8));
        actionTray_icon->setText(QApplication::translate("zuluCrypt", "tray icon", 0, QApplication::UnicodeUTF8));
        actionTray_icon->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+K", 0, QApplication::UnicodeUTF8));
        actionFonts->setText(QApplication::translate("zuluCrypt", "select font", 0, QApplication::UnicodeUTF8));
        actionFonts->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+L", 0, QApplication::UnicodeUTF8));
        actionFavorite_volumes->setText(QApplication::translate("zuluCrypt", "favorite volumes", 0, QApplication::UnicodeUTF8));
        actionManage_favorites->setText(QApplication::translate("zuluCrypt", "manage favorites", 0, QApplication::UnicodeUTF8));
        actionSelect_random_number_generator->setText(QApplication::translate("zuluCrypt", "select random number generator", 0, QApplication::UnicodeUTF8));
        actionSelect_random_number_generator->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionClose_application->setText(QApplication::translate("zuluCrypt", "close application", 0, QApplication::UnicodeUTF8));
        actionClose_application->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        action_minimize->setText(QApplication::translate("zuluCrypt", "minimize", 0, QApplication::UnicodeUTF8));
        action_minimize->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        actionMinimize_to_tray->setText(QApplication::translate("zuluCrypt", "minimize to tray", 0, QApplication::UnicodeUTF8));
        actionMinimize_to_tray->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+Y", 0, QApplication::UnicodeUTF8));
        action_close->setText(QApplication::translate("zuluCrypt", "quit", 0, QApplication::UnicodeUTF8));
        action_close->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionClose_all_opened_volumes->setText(QApplication::translate("zuluCrypt", "close all opened volumes", 0, QApplication::UnicodeUTF8));
        actionClose_all_opened_volumes->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+G", 0, QApplication::UnicodeUTF8));
        actionManage_names->setText(QApplication::translate("zuluCrypt", "manage favorites", 0, QApplication::UnicodeUTF8));
        actionManage_names->setShortcut(QApplication::translate("zuluCrypt", "Ctrl+F", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("zuluCrypt", "Encrypted volume path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("zuluCrypt", "Encrypted volume mount point path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("zuluCrypt", "Type", 0, QApplication::UnicodeUTF8));
        menuOpen->setTitle(QApplication::translate("zuluCrypt", "&open", 0, QApplication::UnicodeUTF8));
        menuCreate->setTitle(QApplication::translate("zuluCrypt", "&create", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("zuluCrypt", "&help", 0, QApplication::UnicodeUTF8));
        menuConfigure->setTitle(QApplication::translate("zuluCrypt", "&luks", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("zuluCrypt", "o&ptions", 0, QApplication::UnicodeUTF8));
        menuFavorites->setTitle(QApplication::translate("zuluCrypt", "&favorites", 0, QApplication::UnicodeUTF8));
        menu_zc->setTitle(QApplication::translate("zuluCrypt", "&zC", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class zuluCrypt: public Ui_zuluCrypt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZULUCRYPT_H
