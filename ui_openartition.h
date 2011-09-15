/********************************************************************************
** Form generated from reading UI file 'openartition.ui'
**
** Created: Thu Sep 15 11:36:53 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENARTITION_H
#define UI_OPENARTITION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_PartitionView
{
public:
    QTableWidget *tableWidgetPartitionView;

    void setupUi(QDialog *PartitionView)
    {
        if (PartitionView->objectName().isEmpty())
            PartitionView->setObjectName(QString::fromUtf8("PartitionView"));
        PartitionView->resize(560, 264);
        tableWidgetPartitionView = new QTableWidget(PartitionView);
        if (tableWidgetPartitionView->columnCount() < 2)
            tableWidgetPartitionView->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setTextAlignment(Qt::AlignHCenter|Qt::AlignTop);
        tableWidgetPartitionView->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetPartitionView->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidgetPartitionView->setObjectName(QString::fromUtf8("tableWidgetPartitionView"));
        tableWidgetPartitionView->setGeometry(QRect(10, 10, 541, 241));
        tableWidgetPartitionView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        retranslateUi(PartitionView);

        QMetaObject::connectSlotsByName(PartitionView);
    } // setupUi

    void retranslateUi(QDialog *PartitionView)
    {
        PartitionView->setWindowTitle(QApplication::translate("PartitionView", "select a partition to open", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetPartitionView->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("PartitionView", "List of partitions", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PartitionView: public Ui_PartitionView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENARTITION_H
