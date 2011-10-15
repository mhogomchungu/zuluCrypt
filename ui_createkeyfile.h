/********************************************************************************
** Form generated from reading UI file 'createkeyfile.ui'
**
** Created: Fri Oct 14 23:12:31 2011
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

    void setupUi(QWidget *createkeyfile)
    {
        if (createkeyfile->objectName().isEmpty())
            createkeyfile->setObjectName(QString::fromUtf8("createkeyfile"));
        createkeyfile->setWindowModality(Qt::ApplicationModal);
        createkeyfile->resize(473, 122);
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
        pbCreate->setGeometry(QRect(150, 80, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCancel = new QPushButton(createkeyfile);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(250, 80, 81, 31));
        pbCancel->setAutoDefault(true);
        pbCancel->setDefault(true);
        label_2 = new QLabel(createkeyfile);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 40, 111, 31));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

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
        pbCreate->setText(QApplication::translate("createkeyfile", "create", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("createkeyfile", "cancel", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("createkeyfile", "destination folder", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class createkeyfile: public Ui_createkeyfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEKEYFILE_H
