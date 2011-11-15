/********************************************************************************
** Form generated from reading UI file 'rngselector.ui'
**
** Created: Tue Nov 15 10:16:41 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RNGSELECTOR_H
#define UI_RNGSELECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rngselector
{
public:
    QGroupBox *groupBox;
    QRadioButton *rbUrandom;
    QRadioButton *rbRandom;
    QPushButton *pbOk;

    void setupUi(QWidget *rngselector)
    {
        if (rngselector->objectName().isEmpty())
            rngselector->setObjectName(QString::fromUtf8("rngselector"));
        rngselector->resize(494, 158);
        groupBox = new QGroupBox(rngselector);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(40, 20, 411, 101));
        rbUrandom = new QRadioButton(groupBox);
        rbUrandom->setObjectName(QString::fromUtf8("rbUrandom"));
        rbUrandom->setGeometry(QRect(50, 10, 381, 31));
        rbRandom = new QRadioButton(groupBox);
        rbRandom->setObjectName(QString::fromUtf8("rbRandom"));
        rbRandom->setGeometry(QRect(50, 40, 361, 51));
        pbOk = new QPushButton(rngselector);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));
        pbOk->setGeometry(QRect(200, 120, 91, 31));
        pbOk->setAutoDefault(false);
        pbOk->setDefault(true);

        retranslateUi(rngselector);

        QMetaObject::connectSlotsByName(rngselector);
    } // setupUi

    void retranslateUi(QWidget *rngselector)
    {
        rngselector->setWindowTitle(QApplication::translate("rngselector", "select a device to use to generate random data", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        rbUrandom->setText(QApplication::translate("rngselector", "/dev/urandom : secure enough and faster", 0, QApplication::UnicodeUTF8));
        rbRandom->setText(QApplication::translate("rngselector", "/dev/random   :  more secure but  slower", 0, QApplication::UnicodeUTF8));
        pbOk->setText(QApplication::translate("rngselector", "&ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class rngselector: public Ui_rngselector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RNGSELECTOR_H
