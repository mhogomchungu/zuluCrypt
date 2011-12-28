/********************************************************************************
** Form generated from reading UI file 'openpartition.ui'
**
** Created: Mon Dec 26 19:36:29 2011
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENPARTITION_H
#define UI_OPENPARTITION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_PartitionView
{
public:
    QTableWidget *tableWidgetPartitionView;
    QCheckBox *checkBoxUUID;

    void setupUi(QDialog *PartitionView)
    {
        if (PartitionView->objectName().isEmpty())
            PartitionView->setObjectName(QString::fromUtf8("PartitionView"));
        PartitionView->setWindowModality(Qt::ApplicationModal);
        PartitionView->resize(680, 327);
        tableWidgetPartitionView = new QTableWidget(PartitionView);
        if (tableWidgetPartitionView->columnCount() < 5)
            tableWidgetPartitionView->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidgetPartitionView->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidgetPartitionView->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        __qtablewidgetitem2->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidgetPartitionView->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        __qtablewidgetitem3->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidgetPartitionView->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        __qtablewidgetitem4->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidgetPartitionView->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tableWidgetPartitionView->setObjectName(QString::fromUtf8("tableWidgetPartitionView"));
        tableWidgetPartitionView->setGeometry(QRect(10, 10, 661, 291));
        tableWidgetPartitionView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetPartitionView->setTabKeyNavigation(false);
        tableWidgetPartitionView->setProperty("showDropIndicator", QVariant(false));
        tableWidgetPartitionView->setDragDropOverwriteMode(false);
        tableWidgetPartitionView->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidgetPartitionView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidgetPartitionView->setShowGrid(false);
        tableWidgetPartitionView->setColumnCount(5);
        tableWidgetPartitionView->horizontalHeader()->setVisible(false);
        tableWidgetPartitionView->verticalHeader()->setVisible(true);
        checkBoxUUID = new QCheckBox(PartitionView);
        checkBoxUUID->setObjectName(QString::fromUtf8("checkBoxUUID"));
        checkBoxUUID->setEnabled(true);
        checkBoxUUID->setGeometry(QRect(300, 300, 121, 21));
        QWidget::setTabOrder(tableWidgetPartitionView, checkBoxUUID);

        retranslateUi(PartitionView);

        QMetaObject::connectSlotsByName(PartitionView);
    } // setupUi

    void retranslateUi(QDialog *PartitionView)
    {
        PartitionView->setWindowTitle(QApplication::translate("PartitionView", "select a partition to open", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetPartitionView->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("PartitionView", "partition", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetPartitionView->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("PartitionView", "size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetPartitionView->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("PartitionView", "label", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidgetPartitionView->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("PartitionView", "type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidgetPartitionView->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("PartitionView", "uuid", 0, QApplication::UnicodeUTF8));
        checkBoxUUID->setText(QApplication::translate("PartitionView", "use uuid", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PartitionView: public Ui_PartitionView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENPARTITION_H
