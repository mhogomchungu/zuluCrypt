/********************************************************************************
** Form generated from reading UI file 'dialogmsg.ui'
**
** Created: Thu May 24 04:13:15 2012
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

    void setupUi(QDialog *DialogMsg)
    {
        if (DialogMsg->objectName().isEmpty())
            DialogMsg->setObjectName(QString::fromUtf8("DialogMsg"));
        DialogMsg->resize(239, 234);
        pbOk = new QPushButton(DialogMsg);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));
        pbOk->setGeometry(QRect(80, 200, 81, 31));
        pbYes = new QPushButton(DialogMsg);
        pbYes->setObjectName(QString::fromUtf8("pbYes"));
        pbYes->setGeometry(QRect(10, 200, 71, 31));
        pbNo = new QPushButton(DialogMsg);
        pbNo->setObjectName(QString::fromUtf8("pbNo"));
        pbNo->setGeometry(QRect(160, 200, 75, 31));
        label = new QLabel(DialogMsg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 0, 221, 191));
        label->setTextFormat(Qt::PlainText);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        retranslateUi(DialogMsg);

        QMetaObject::connectSlotsByName(DialogMsg);
    } // setupUi

    void retranslateUi(QDialog *DialogMsg)
    {
        DialogMsg->setWindowTitle(QApplication::translate("DialogMsg", "Dialog", 0, QApplication::UnicodeUTF8));
        pbOk->setText(QApplication::translate("DialogMsg", "ok", 0, QApplication::UnicodeUTF8));
        pbYes->setText(QApplication::translate("DialogMsg", "yes", 0, QApplication::UnicodeUTF8));
        pbNo->setText(QApplication::translate("DialogMsg", "no", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DialogMsg: public Ui_DialogMsg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGMSG_H
