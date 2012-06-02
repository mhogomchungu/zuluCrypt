/********************************************************************************
** Form generated from reading UI file 'openpartition.ui'
**
** Created: Sat Jun 2 14:42:56 2012
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
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_PartitionView
{
public:
    QTableWidget *tableWidget;
    QCheckBox *checkBoxUUID;
    QPushButton *pbHelp;
    QPushButton *pbUUID;
    QPushButton *pbCancel;

    void setupUi(QDialog *PartitionView)
    {
        if (PartitionView->objectName().isEmpty())
            PartitionView->setObjectName(QString::fromUtf8("PartitionView"));
        PartitionView->setWindowModality(Qt::ApplicationModal);
        PartitionView->resize(680, 348);
        tableWidget = new QTableWidget(PartitionView);
        if (tableWidget->columnCount() < 5)
            tableWidget->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        __qtablewidgetitem2->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        __qtablewidgetitem3->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        __qtablewidgetitem4->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 10, 661, 291));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setTabKeyNavigation(false);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setShowGrid(false);
        tableWidget->setColumnCount(5);
        tableWidget->horizontalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setVisible(false);
        checkBoxUUID = new QCheckBox(PartitionView);
        checkBoxUUID->setObjectName(QString::fromUtf8("checkBoxUUID"));
        checkBoxUUID->setEnabled(true);
        checkBoxUUID->setGeometry(QRect(560, 320, 71, 31));
        pbHelp = new QPushButton(PartitionView);
        pbHelp->setObjectName(QString::fromUtf8("pbHelp"));
        pbHelp->setGeometry(QRect(220, 310, 75, 31));
        pbUUID = new QPushButton(PartitionView);
        pbUUID->setObjectName(QString::fromUtf8("pbUUID"));
        pbUUID->setGeometry(QRect(380, 310, 75, 31));
        pbCancel = new QPushButton(PartitionView);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(300, 310, 75, 31));
        QWidget::setTabOrder(tableWidget, checkBoxUUID);

        retranslateUi(PartitionView);

        QMetaObject::connectSlotsByName(PartitionView);
    } // setupUi

    void retranslateUi(QDialog *PartitionView)
    {
        PartitionView->setWindowTitle(QApplication::translate("PartitionView", "select a partition to open", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("PartitionView", "partition", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("PartitionView", "size", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("PartitionView", "label", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("PartitionView", "type", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("PartitionView", "uuid", 0, QApplication::UnicodeUTF8));
        checkBoxUUID->setText(QApplication::translate("PartitionView", "use uuid", 0, QApplication::UnicodeUTF8));
        pbHelp->setText(QApplication::translate("PartitionView", "&help", 0, QApplication::UnicodeUTF8));
        pbUUID->setText(QApplication::translate("PartitionView", "use &uuid", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("PartitionView", "&cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PartitionView: public Ui_PartitionView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENPARTITION_H
