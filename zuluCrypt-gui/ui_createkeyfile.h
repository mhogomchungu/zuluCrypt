/********************************************************************************
** Form generated from reading UI file 'createkeyfile.ui'
**
** Created: Sat Nov 5 02:49:56 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEKEYFILE_H
#define UI_CREATEKEYFILE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_createkeyfile
{
public:
    QLineEdit *lineEditFileName;
    QLabel *label;
    QLineEdit *lineEditPath;
    QPushButton *pbOpenFolder;
    QPushButton *pbCreate;
    QPushButton *pbCancel;
    QLabel *label_2;
    QLabel *labelRNG;
    QComboBox *comboBoxRNG;

    void setupUi(QWidget *createkeyfile)
    {
        if (createkeyfile->objectName().isEmpty())
            createkeyfile->setObjectName(QString::fromUtf8("createkeyfile"));
        createkeyfile->setWindowModality(Qt::ApplicationModal);
        createkeyfile->resize(473, 138);
        lineEditFileName = new QLineEdit(createkeyfile);
        lineEditFileName->setObjectName(QString::fromUtf8("lineEditFileName"));
        lineEditFileName->setGeometry(QRect(110, 10, 321, 31));
        label = new QLabel(createkeyfile);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 91, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditPath = new QLineEdit(createkeyfile);
        lineEditPath->setObjectName(QString::fromUtf8("lineEditPath"));
        lineEditPath->setGeometry(QRect(110, 40, 321, 31));
        pbOpenFolder = new QPushButton(createkeyfile);
        pbOpenFolder->setObjectName(QString::fromUtf8("pbOpenFolder"));
        pbOpenFolder->setGeometry(QRect(430, 40, 31, 31));
        pbCreate = new QPushButton(createkeyfile);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(150, 100, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCreate->setDefault(true);
        pbCancel = new QPushButton(createkeyfile);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(240, 100, 81, 31));
        pbCancel->setAutoDefault(false);
        pbCancel->setDefault(false);
        label_2 = new QLabel(createkeyfile);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 40, 111, 31));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelRNG = new QLabel(createkeyfile);
        labelRNG->setObjectName(QString::fromUtf8("labelRNG"));
        labelRNG->setGeometry(QRect(10, 70, 101, 31));
        labelRNG->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        comboBoxRNG = new QComboBox(createkeyfile);
        comboBoxRNG->setObjectName(QString::fromUtf8("comboBoxRNG"));
        comboBoxRNG->setGeometry(QRect(110, 70, 111, 31));

        retranslateUi(createkeyfile);

        QMetaObject::connectSlotsByName(createkeyfile);
    } // setupUi

    void retranslateUi(QWidget *createkeyfile)
    {
        createkeyfile->setWindowTitle(QApplication::translate("createkeyfile", "create a key file", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("createkeyfile", "key file name", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lineEditPath->setToolTip(QApplication::translate("createkeyfile", "path to a folder to create a key in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pbOpenFolder->setToolTip(QApplication::translate("createkeyfile", "open a folder a key file will be created in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbOpenFolder->setText(QString());
        pbCreate->setText(QApplication::translate("createkeyfile", "c&reate", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("createkeyfile", "&cancel", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("createkeyfile", "destination folder", 0, QApplication::UnicodeUTF8));
        labelRNG->setText(QApplication::translate("createkeyfile", "rng", 0, QApplication::UnicodeUTF8));
        comboBoxRNG->clear();
        comboBoxRNG->insertItems(0, QStringList()
         << QApplication::translate("createkeyfile", "/dev/random", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("createkeyfile", "/dev/urandom", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class createkeyfile: public Ui_createkeyfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEKEYFILE_H
