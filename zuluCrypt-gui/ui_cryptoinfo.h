/********************************************************************************
** Form generated from reading UI file 'cryptoinfo.ui'
**
** Created: Mon Jul 2 03:08:33 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRYPTOINFO_H
#define UI_CRYPTOINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cryptoinfo
{
public:
    QPushButton *pbOK;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_12;
    QFrame *line;
    QLabel *label_14;
    QFrame *line_2;

    void setupUi(QWidget *cryptoinfo)
    {
        if (cryptoinfo->objectName().isEmpty())
            cryptoinfo->setObjectName(QString::fromUtf8("cryptoinfo"));
        cryptoinfo->setWindowModality(Qt::ApplicationModal);
        cryptoinfo->resize(482, 228);
        pbOK = new QPushButton(cryptoinfo);
        pbOK->setObjectName(QString::fromUtf8("pbOK"));
        pbOK->setGeometry(QRect(200, 190, 91, 31));
        label = new QLabel(cryptoinfo);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 30, 481, 31));
        label->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(cryptoinfo);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 50, 481, 31));
        label_3->setAlignment(Qt::AlignCenter);
        label_4 = new QLabel(cryptoinfo);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(0, 70, 481, 31));
        label_4->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(cryptoinfo);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 90, 481, 31));
        label_5->setAlignment(Qt::AlignCenter);
        label_9 = new QLabel(cryptoinfo);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(0, 0, 481, 31));
        label_9->setAlignment(Qt::AlignCenter);
        label_10 = new QLabel(cryptoinfo);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(0, 120, 481, 31));
        label_10->setAlignment(Qt::AlignCenter);
        label_12 = new QLabel(cryptoinfo);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(0, 140, 481, 31));
        label_12->setAlignment(Qt::AlignCenter);
        line = new QFrame(cryptoinfo);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(20, 110, 461, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label_14 = new QLabel(cryptoinfo);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(0, 160, 481, 31));
        label_14->setAlignment(Qt::AlignCenter);
        line_2 = new QFrame(cryptoinfo);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(20, 20, 461, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        retranslateUi(cryptoinfo);

        QMetaObject::connectSlotsByName(cryptoinfo);
    } // setupUi

    void retranslateUi(QWidget *cryptoinfo)
    {
        cryptoinfo->setWindowTitle(QApplication::translate("cryptoinfo", "crypo info", 0, QApplication::UnicodeUTF8));
        pbOK->setText(QApplication::translate("cryptoinfo", "ok", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("cryptoinfo", "options for volumes of type plain:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("cryptoinfo", "cipher: aes-cbc-essiv:sha256", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("cryptoinfo", "keysize: 256 bits", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("cryptoinfo", "password hash: ripemd160", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("cryptoinfo", "all volumes are created and opened with the following options:", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("cryptoinfo", "options for volumes of type luks1", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("cryptoinfo", "keysize: 256 bits", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("cryptoinfo", "luks header hashing: sha1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class cryptoinfo: public Ui_cryptoinfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRYPTOINFO_H
