/********************************************************************************
** Form generated from reading UI file 'dialogmsg.ui'
**
** Created: Mon Jun 11 06:58:58 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGMSG_H
#define UI_DIALOGMSG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DialogMsg
{
public:
    QPushButton *pbOk;
    QPushButton *pbYes;
    QPushButton *pbNo;
    QLabel *label;
    QLabel *labelType;
    QLabel *labelCipher;
    QLabel *labelKeySize;
    QLabel *labelDevice;
    QLabel *labelLoop;
    QLabel *labelOffset;
    QLabel *labelSize;
    QLabel *labelMode;
    QLabel *labelFs;
    QLabel *labelSize_2;
    QLabel *labelUsed;
    QLabel *labelUnused;
    QLabel *labelUsedPerc;
    QLabel *labelActiveSlots;

    void setupUi(QDialog *DialogMsg)
    {
        if (DialogMsg->objectName().isEmpty())
            DialogMsg->setObjectName(QString::fromUtf8("DialogMsg"));
        DialogMsg->resize(321, 344);
        pbOk = new QPushButton(DialogMsg);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));
        pbOk->setGeometry(QRect(120, 310, 81, 31));
        pbYes = new QPushButton(DialogMsg);
        pbYes->setObjectName(QString::fromUtf8("pbYes"));
        pbYes->setGeometry(QRect(40, 310, 71, 31));
        pbNo = new QPushButton(DialogMsg);
        pbNo->setObjectName(QString::fromUtf8("pbNo"));
        pbNo->setGeometry(QRect(210, 310, 75, 31));
        label = new QLabel(DialogMsg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(260, 240, 41, 31));
        label->setTextFormat(Qt::PlainText);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);
        labelType = new QLabel(DialogMsg);
        labelType->setObjectName(QString::fromUtf8("labelType"));
        labelType->setGeometry(QRect(0, 20, 321, 21));
        labelType->setAlignment(Qt::AlignCenter);
        labelCipher = new QLabel(DialogMsg);
        labelCipher->setObjectName(QString::fromUtf8("labelCipher"));
        labelCipher->setGeometry(QRect(0, 40, 321, 21));
        labelCipher->setAlignment(Qt::AlignCenter);
        labelKeySize = new QLabel(DialogMsg);
        labelKeySize->setObjectName(QString::fromUtf8("labelKeySize"));
        labelKeySize->setGeometry(QRect(0, 60, 321, 21));
        labelKeySize->setAlignment(Qt::AlignCenter);
        labelDevice = new QLabel(DialogMsg);
        labelDevice->setObjectName(QString::fromUtf8("labelDevice"));
        labelDevice->setGeometry(QRect(0, 80, 321, 21));
        labelDevice->setAlignment(Qt::AlignCenter);
        labelLoop = new QLabel(DialogMsg);
        labelLoop->setObjectName(QString::fromUtf8("labelLoop"));
        labelLoop->setGeometry(QRect(0, 100, 321, 21));
        labelLoop->setAlignment(Qt::AlignCenter);
        labelOffset = new QLabel(DialogMsg);
        labelOffset->setObjectName(QString::fromUtf8("labelOffset"));
        labelOffset->setGeometry(QRect(0, 120, 321, 21));
        labelOffset->setAlignment(Qt::AlignCenter);
        labelSize = new QLabel(DialogMsg);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));
        labelSize->setGeometry(QRect(0, 140, 321, 21));
        labelSize->setAlignment(Qt::AlignCenter);
        labelMode = new QLabel(DialogMsg);
        labelMode->setObjectName(QString::fromUtf8("labelMode"));
        labelMode->setGeometry(QRect(0, 160, 321, 21));
        labelMode->setAlignment(Qt::AlignCenter);
        labelFs = new QLabel(DialogMsg);
        labelFs->setObjectName(QString::fromUtf8("labelFs"));
        labelFs->setGeometry(QRect(0, 200, 321, 21));
        labelFs->setAlignment(Qt::AlignCenter);
        labelSize_2 = new QLabel(DialogMsg);
        labelSize_2->setObjectName(QString::fromUtf8("labelSize_2"));
        labelSize_2->setGeometry(QRect(0, 220, 321, 21));
        labelSize_2->setAlignment(Qt::AlignCenter);
        labelUsed = new QLabel(DialogMsg);
        labelUsed->setObjectName(QString::fromUtf8("labelUsed"));
        labelUsed->setGeometry(QRect(0, 240, 321, 20));
        labelUsed->setAlignment(Qt::AlignCenter);
        labelUnused = new QLabel(DialogMsg);
        labelUnused->setObjectName(QString::fromUtf8("labelUnused"));
        labelUnused->setGeometry(QRect(0, 260, 321, 20));
        labelUnused->setAlignment(Qt::AlignCenter);
        labelUsedPerc = new QLabel(DialogMsg);
        labelUsedPerc->setObjectName(QString::fromUtf8("labelUsedPerc"));
        labelUsedPerc->setGeometry(QRect(0, 280, 321, 20));
        labelUsedPerc->setAlignment(Qt::AlignCenter);
        labelActiveSlots = new QLabel(DialogMsg);
        labelActiveSlots->setObjectName(QString::fromUtf8("labelActiveSlots"));
        labelActiveSlots->setGeometry(QRect(0, 180, 321, 20));
        labelActiveSlots->setAlignment(Qt::AlignCenter);

        retranslateUi(DialogMsg);

        QMetaObject::connectSlotsByName(DialogMsg);
    } // setupUi

    void retranslateUi(QDialog *DialogMsg)
    {
        DialogMsg->setWindowTitle(QApplication::translate("DialogMsg", "Dialog", 0, QApplication::UnicodeUTF8));
        pbOk->setText(QApplication::translate("DialogMsg", "&ok", 0, QApplication::UnicodeUTF8));
        pbYes->setText(QApplication::translate("DialogMsg", "&yes", 0, QApplication::UnicodeUTF8));
        pbNo->setText(QApplication::translate("DialogMsg", "&no", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DialogMsg", "text", 0, QApplication::UnicodeUTF8));
        labelType->setText(QApplication::translate("DialogMsg", "type", 0, QApplication::UnicodeUTF8));
        labelCipher->setText(QApplication::translate("DialogMsg", "cipher", 0, QApplication::UnicodeUTF8));
        labelKeySize->setText(QApplication::translate("DialogMsg", "key size", 0, QApplication::UnicodeUTF8));
        labelDevice->setText(QApplication::translate("DialogMsg", "device", 0, QApplication::UnicodeUTF8));
        labelLoop->setText(QApplication::translate("DialogMsg", "loop", 0, QApplication::UnicodeUTF8));
        labelOffset->setText(QApplication::translate("DialogMsg", "offset", 0, QApplication::UnicodeUTF8));
        labelSize->setText(QApplication::translate("DialogMsg", "size", 0, QApplication::UnicodeUTF8));
        labelMode->setText(QApplication::translate("DialogMsg", "mode", 0, QApplication::UnicodeUTF8));
        labelFs->setText(QApplication::translate("DialogMsg", "fs", 0, QApplication::UnicodeUTF8));
        labelSize_2->setText(QApplication::translate("DialogMsg", "size", 0, QApplication::UnicodeUTF8));
        labelUsed->setText(QApplication::translate("DialogMsg", "used", 0, QApplication::UnicodeUTF8));
        labelUnused->setText(QApplication::translate("DialogMsg", "unused", 0, QApplication::UnicodeUTF8));
        labelUsedPerc->setText(QApplication::translate("DialogMsg", "used %", 0, QApplication::UnicodeUTF8));
        labelActiveSlots->setText(QApplication::translate("DialogMsg", "active slots", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DialogMsg: public Ui_DialogMsg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGMSG_H
