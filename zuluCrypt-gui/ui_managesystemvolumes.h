/********************************************************************************
** Form generated from reading UI file 'managesystemvolumes.ui'
**
** Created: Tue Jul 17 02:12:22 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGESYSTEMVOLUMES_H
#define UI_MANAGESYSTEMVOLUMES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_manageSystemVolumes
{
public:
    QPushButton *pbAdd;
    QPushButton *pbDone;
    QLineEdit *volumePath;
    QPushButton *pbFile;
    QPushButton *pbPartition;
    QLabel *label;
    QTableWidget *tableWidget;

    void setupUi(QDialog *manageSystemVolumes)
    {
        if (manageSystemVolumes->objectName().isEmpty())
            manageSystemVolumes->setObjectName(QString::fromUtf8("manageSystemVolumes"));
        manageSystemVolumes->setWindowModality(Qt::WindowModal);
        manageSystemVolumes->resize(522, 314);
        pbAdd = new QPushButton(manageSystemVolumes);
        pbAdd->setObjectName(QString::fromUtf8("pbAdd"));
        pbAdd->setGeometry(QRect(180, 280, 81, 31));
        pbDone = new QPushButton(manageSystemVolumes);
        pbDone->setObjectName(QString::fromUtf8("pbDone"));
        pbDone->setGeometry(QRect(260, 280, 81, 31));
        volumePath = new QLineEdit(manageSystemVolumes);
        volumePath->setObjectName(QString::fromUtf8("volumePath"));
        volumePath->setGeometry(QRect(110, 250, 321, 31));
        pbFile = new QPushButton(manageSystemVolumes);
        pbFile->setObjectName(QString::fromUtf8("pbFile"));
        pbFile->setGeometry(QRect(430, 250, 31, 31));
        pbPartition = new QPushButton(manageSystemVolumes);
        pbPartition->setObjectName(QString::fromUtf8("pbPartition"));
        pbPartition->setGeometry(QRect(460, 250, 31, 31));
        label = new QLabel(manageSystemVolumes);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 250, 111, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        tableWidget = new QTableWidget(manageSystemVolumes);
        if (tableWidget->columnCount() < 1)
            tableWidget->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 10, 501, 241));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidget->setShowGrid(false);
        tableWidget->setGridStyle(Qt::NoPen);

        retranslateUi(manageSystemVolumes);

        QMetaObject::connectSlotsByName(manageSystemVolumes);
    } // setupUi

    void retranslateUi(QDialog *manageSystemVolumes)
    {
        manageSystemVolumes->setWindowTitle(QApplication::translate("manageSystemVolumes", "manage system volumes", 0, QApplication::UnicodeUTF8));
        pbAdd->setText(QApplication::translate("manageSystemVolumes", "&add", 0, QApplication::UnicodeUTF8));
        pbDone->setText(QApplication::translate("manageSystemVolumes", "&done", 0, QApplication::UnicodeUTF8));
        pbFile->setText(QString());
        pbPartition->setText(QString());
        label->setText(QApplication::translate("manageSystemVolumes", "volume path: ", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("manageSystemVolumes", "path to system volumes", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class manageSystemVolumes: public Ui_manageSystemVolumes {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGESYSTEMVOLUMES_H
