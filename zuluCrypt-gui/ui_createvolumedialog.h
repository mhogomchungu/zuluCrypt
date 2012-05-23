/********************************************************************************
** Form generated from reading UI file 'createvolumedialog.ui'
**
** Created: Wed May 23 18:31:26 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEVOLUMEDIALOG_H
#define UI_CREATEVOLUMEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_CreateVolumeDialog
{
public:
    QPushButton *pbYes;
    QPushButton *pbNo;
    QLabel *label_1;

    void setupUi(QDialog *CreateVolumeDialog)
    {
        if (CreateVolumeDialog->objectName().isEmpty())
            CreateVolumeDialog->setObjectName(QString::fromUtf8("CreateVolumeDialog"));
        CreateVolumeDialog->resize(461, 195);
        pbYes = new QPushButton(CreateVolumeDialog);
        pbYes->setObjectName(QString::fromUtf8("pbYes"));
        pbYes->setGeometry(QRect(150, 160, 81, 31));
        pbNo = new QPushButton(CreateVolumeDialog);
        pbNo->setObjectName(QString::fromUtf8("pbNo"));
        pbNo->setGeometry(QRect(230, 160, 81, 31));
        label_1 = new QLabel(CreateVolumeDialog);
        label_1->setObjectName(QString::fromUtf8("label_1"));
        label_1->setGeometry(QRect(10, 10, 441, 151));
        label_1->setAlignment(Qt::AlignCenter);
        label_1->setWordWrap(true);

        retranslateUi(CreateVolumeDialog);

        QMetaObject::connectSlotsByName(CreateVolumeDialog);
    } // setupUi

    void retranslateUi(QDialog *CreateVolumeDialog)
    {
        CreateVolumeDialog->setWindowTitle(QApplication::translate("CreateVolumeDialog", "warning!!", 0, QApplication::UnicodeUTF8));
        pbYes->setText(QApplication::translate("CreateVolumeDialog", "yes", 0, QApplication::UnicodeUTF8));
        pbNo->setText(QApplication::translate("CreateVolumeDialog", "no", 0, QApplication::UnicodeUTF8));
        label_1->setText(QApplication::translate("CreateVolumeDialog", "This operation will lead to permanent destrunction of all present data in /dev/sdc1. Are you sure you want to continue?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CreateVolumeDialog: public Ui_CreateVolumeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEVOLUMEDIALOG_H
