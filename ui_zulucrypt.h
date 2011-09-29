/********************************************************************************
** Form generated from reading UI file 'zulucrypt.ui'
**
** Created: Thu Sep 29 13:55:59 2011
**      by: Qt User Interface Compiler version 4.7.4
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
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QMenuBar *menuBar;
    QMenu *menuOpen;
    QMenu *menuCreate;
    QMenu *menuHelp;
    QMenu *menuConfigure;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *zuluCrypt)
    {
        if (zuluCrypt->objectName().isEmpty())
            zuluCrypt->setObjectName(QString::fromUtf8("zuluCrypt"));
        zuluCrypt->setWindowModality(Qt::ApplicationModal);
        zuluCrypt->setEnabled(true);
        zuluCrypt->resize(659, 375);
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
        tableWidget->setGeometry(QRect(10, 0, 641, 331));
        tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableWidget->setFrameShadow(QFrame::Raised);
        tableWidget->setMidLineWidth(1);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setGridStyle(Qt::NoPen);
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(3);
        zuluCrypt->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(zuluCrypt);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 659, 20));
        menuOpen = new QMenu(menuBar);
        menuOpen->setObjectName(QString::fromUtf8("menuOpen"));
        menuCreate = new QMenu(menuBar);
        menuCreate->setObjectName(QString::fromUtf8("menuCreate"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuConfigure = new QMenu(menuBar);
        menuConfigure->setObjectName(QString::fromUtf8("menuConfigure"));
        zuluCrypt->setMenuBar(menuBar);
        statusBar = new QStatusBar(zuluCrypt);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        zuluCrypt->setStatusBar(statusBar);

        menuBar->addAction(menuOpen->menuAction());
        menuBar->addAction(menuCreate->menuAction());
        menuBar->addAction(menuConfigure->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuOpen->addAction(actionFileOpen);
        menuOpen->addAction(actionPartitionOpen);
        menuCreate->addAction(actionFileCreate);
        menuCreate->addAction(actionPartitionCreate);
        menuHelp->addAction(actionInfo);
        menuHelp->addAction(actionAbout);
        menuConfigure->addAction(actionAddKey);
        menuConfigure->addAction(actionDeleteKey);

        retranslateUi(zuluCrypt);

        QMetaObject::connectSlotsByName(zuluCrypt);
    } // setupUi

    void retranslateUi(QMainWindow *zuluCrypt)
    {
        zuluCrypt->setWindowTitle(QApplication::translate("zuluCrypt", "zuluCrypt", 0, QApplication::UnicodeUTF8));
        actionFileOpen->setText(QApplication::translate("zuluCrypt", "encrypted file", 0, QApplication::UnicodeUTF8));
        actionPartitionOpen->setText(QApplication::translate("zuluCrypt", "encrypted partition", 0, QApplication::UnicodeUTF8));
        actionFileCreate->setText(QApplication::translate("zuluCrypt", "encrypted file", 0, QApplication::UnicodeUTF8));
        actionPartitionCreate->setText(QApplication::translate("zuluCrypt", "encrypted partition", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("zuluCrypt", "about", 0, QApplication::UnicodeUTF8));
        actionAddKey->setText(QApplication::translate("zuluCrypt", "add key", 0, QApplication::UnicodeUTF8));
        actionDeleteKey->setText(QApplication::translate("zuluCrypt", "delete key", 0, QApplication::UnicodeUTF8));
        actionInfo->setText(QApplication::translate("zuluCrypt", "crypto info", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("zuluCrypt", "Encrypted volume path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("zuluCrypt", "Encrypted volume mount point path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("zuluCrypt", "Type", 0, QApplication::UnicodeUTF8));
        menuOpen->setTitle(QApplication::translate("zuluCrypt", "open", 0, QApplication::UnicodeUTF8));
        menuCreate->setTitle(QApplication::translate("zuluCrypt", "create", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("zuluCrypt", "help", 0, QApplication::UnicodeUTF8));
        menuConfigure->setTitle(QApplication::translate("zuluCrypt", "luks", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class zuluCrypt: public Ui_zuluCrypt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZULUCRYPT_H
