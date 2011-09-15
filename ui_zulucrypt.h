/********************************************************************************
** Form generated from reading UI file 'zulucrypt.ui'
**
** Created: Thu Sep 15 11:36:53 2011
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
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QMenuBar *menuBar;
    QMenu *menuOpen;
    QMenu *menuCreate;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *zuluCrypt)
    {
        if (zuluCrypt->objectName().isEmpty())
            zuluCrypt->setObjectName(QString::fromUtf8("zuluCrypt"));
        zuluCrypt->setWindowModality(Qt::ApplicationModal);
        zuluCrypt->setEnabled(true);
        zuluCrypt->resize(659, 375);
        actionFileOpen = new QAction(zuluCrypt);
        actionFileOpen->setObjectName(QString::fromUtf8("actionFileOpen"));
        actionPartitionOpen = new QAction(zuluCrypt);
        actionPartitionOpen->setObjectName(QString::fromUtf8("actionPartitionOpen"));
        actionFileCreate = new QAction(zuluCrypt);
        actionFileCreate->setObjectName(QString::fromUtf8("actionFileCreate"));
        actionFileCreate->setEnabled(false);
        actionFileCreate->setVisible(true);
        actionPartitionCreate = new QAction(zuluCrypt);
        actionPartitionCreate->setObjectName(QString::fromUtf8("actionPartitionCreate"));
        actionPartitionCreate->setEnabled(false);
        actionPartitionCreate->setVisible(true);
        actionAbout = new QAction(zuluCrypt);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralWidget = new QWidget(zuluCrypt);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 0, 641, 331));
        tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableWidget->setFrameShadow(QFrame::Raised);
        tableWidget->setMidLineWidth(1);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setGridStyle(Qt::NoPen);
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(2);
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
        zuluCrypt->setMenuBar(menuBar);
        statusBar = new QStatusBar(zuluCrypt);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        zuluCrypt->setStatusBar(statusBar);

        menuBar->addAction(menuOpen->menuAction());
        menuBar->addAction(menuCreate->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuOpen->addAction(actionFileOpen);
        menuOpen->addAction(actionPartitionOpen);
        menuCreate->addAction(actionFileCreate);
        menuCreate->addAction(actionPartitionCreate);
        menuHelp->addAction(actionAbout);

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
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("zuluCrypt", "Encrypted volume path", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("zuluCrypt", "Encrypted volume mount point path", 0, QApplication::UnicodeUTF8));
        menuOpen->setTitle(QApplication::translate("zuluCrypt", "open", 0, QApplication::UnicodeUTF8));
        menuCreate->setTitle(QApplication::translate("zuluCrypt", "create", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("zuluCrypt", "help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class zuluCrypt: public Ui_zuluCrypt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZULUCRYPT_H
