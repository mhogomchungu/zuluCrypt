/********************************************************************************
** Form generated from reading UI file 'managedevicenames.ui'
**
** Created: Mon Jan 9 21:55:07 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGEDEVICENAMES_H
#define UI_MANAGEDEVICENAMES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_managedevicenames
{
public:
    QPushButton *pbAdd;
    QTableWidget *tableWidget;
    QLabel *labelName;
    QLabel *labelDeviceAddress;
    QLineEdit *lineEditMountPath;
    QLineEdit *lineEditDeviceAddress;
    QPushButton *pbDeviceAddress;
    QPushButton *pbFileAddress;
    QPushButton *pbCancel;

    void setupUi(QWidget *managedevicenames)
    {
        if (managedevicenames->objectName().isEmpty())
            managedevicenames->setObjectName(QString::fromUtf8("managedevicenames"));
        managedevicenames->resize(651, 323);
        pbAdd = new QPushButton(managedevicenames);
        pbAdd->setObjectName(QString::fromUtf8("pbAdd"));
        pbAdd->setGeometry(QRect(240, 290, 75, 31));
        tableWidget = new QTableWidget(managedevicenames);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 10, 631, 221));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setTabKeyNavigation(false);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidget->setShowGrid(false);
        labelName = new QLabel(managedevicenames);
        labelName->setObjectName(QString::fromUtf8("labelName"));
        labelName->setGeometry(QRect(30, 230, 131, 31));
        labelName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelDeviceAddress = new QLabel(managedevicenames);
        labelDeviceAddress->setObjectName(QString::fromUtf8("labelDeviceAddress"));
        labelDeviceAddress->setGeometry(QRect(60, 260, 101, 31));
        labelDeviceAddress->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditMountPath = new QLineEdit(managedevicenames);
        lineEditMountPath->setObjectName(QString::fromUtf8("lineEditMountPath"));
        lineEditMountPath->setGeometry(QRect(170, 260, 301, 31));
        lineEditDeviceAddress = new QLineEdit(managedevicenames);
        lineEditDeviceAddress->setObjectName(QString::fromUtf8("lineEditDeviceAddress"));
        lineEditDeviceAddress->setGeometry(QRect(170, 230, 301, 31));
        pbDeviceAddress = new QPushButton(managedevicenames);
        pbDeviceAddress->setObjectName(QString::fromUtf8("pbDeviceAddress"));
        pbDeviceAddress->setGeometry(QRect(470, 230, 31, 31));
        pbFileAddress = new QPushButton(managedevicenames);
        pbFileAddress->setObjectName(QString::fromUtf8("pbFileAddress"));
        pbFileAddress->setGeometry(QRect(500, 230, 31, 31));
        pbCancel = new QPushButton(managedevicenames);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(320, 290, 75, 31));
        QWidget::setTabOrder(tableWidget, lineEditDeviceAddress);
        QWidget::setTabOrder(lineEditDeviceAddress, pbDeviceAddress);
        QWidget::setTabOrder(pbDeviceAddress, pbFileAddress);
        QWidget::setTabOrder(pbFileAddress, lineEditMountPath);
        QWidget::setTabOrder(lineEditMountPath, pbAdd);
        QWidget::setTabOrder(pbAdd, pbCancel);

        retranslateUi(managedevicenames);

        QMetaObject::connectSlotsByName(managedevicenames);
    } // setupUi

    void retranslateUi(QWidget *managedevicenames)
    {
        managedevicenames->setWindowTitle(QApplication::translate("managedevicenames", "manage favorites", 0, QApplication::UnicodeUTF8));
        pbAdd->setText(QApplication::translate("managedevicenames", "&add", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("managedevicenames", "device address", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("managedevicenames", "mount point path", 0, QApplication::UnicodeUTF8));
        labelName->setText(QApplication::translate("managedevicenames", "device address", 0, QApplication::UnicodeUTF8));
        labelDeviceAddress->setText(QApplication::translate("managedevicenames", "mount point path", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pbDeviceAddress->setToolTip(QApplication::translate("managedevicenames", "open partition dialog", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbDeviceAddress->setText(QString());
#ifndef QT_NO_TOOLTIP
        pbFileAddress->setToolTip(QApplication::translate("managedevicenames", "open file dialog", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbFileAddress->setText(QString());
        pbCancel->setText(QApplication::translate("managedevicenames", "&done", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class managedevicenames: public Ui_managedevicenames {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGEDEVICENAMES_H
