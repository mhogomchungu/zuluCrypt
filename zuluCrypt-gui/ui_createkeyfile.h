/********************************************************************************
** Form generated from reading UI file 'createkeyfile.ui'
**
** Created: Thu May 3 18:27:09 2012
**      by: Qt User Interface Compiler version 4.8.0
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
        createkeyfile->resize(551, 138);
        lineEditFileName = new QLineEdit(createkeyfile);
        lineEditFileName->setObjectName(QString::fromUtf8("lineEditFileName"));
        lineEditFileName->setGeometry(QRect(120, 10, 311, 31));
        label = new QLabel(createkeyfile);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 10, 111, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditPath = new QLineEdit(createkeyfile);
        lineEditPath->setObjectName(QString::fromUtf8("lineEditPath"));
        lineEditPath->setGeometry(QRect(120, 40, 311, 31));
        pbOpenFolder = new QPushButton(createkeyfile);
        pbOpenFolder->setObjectName(QString::fromUtf8("pbOpenFolder"));
        pbOpenFolder->setGeometry(QRect(430, 40, 31, 31));
        pbCreate = new QPushButton(createkeyfile);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(190, 100, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCreate->setDefault(true);
        pbCancel = new QPushButton(createkeyfile);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(280, 100, 81, 31));
        pbCancel->setAutoDefault(false);
        pbCancel->setDefault(false);
        label_2 = new QLabel(createkeyfile);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 40, 111, 31));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelRNG = new QLabel(createkeyfile);
        labelRNG->setObjectName(QString::fromUtf8("labelRNG"));
        labelRNG->setGeometry(QRect(0, 70, 111, 31));
        labelRNG->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        comboBoxRNG = new QComboBox(createkeyfile);
        comboBoxRNG->insertItems(0, QStringList()
         << QString::fromUtf8("/dev/random")
         << QString::fromUtf8("/dev/urandom")
        );
        comboBoxRNG->setObjectName(QString::fromUtf8("comboBoxRNG"));
        comboBoxRNG->setGeometry(QRect(120, 70, 161, 31));
        QWidget::setTabOrder(lineEditFileName, lineEditPath);
        QWidget::setTabOrder(lineEditPath, pbOpenFolder);
        QWidget::setTabOrder(pbOpenFolder, comboBoxRNG);
        QWidget::setTabOrder(comboBoxRNG, pbCreate);
        QWidget::setTabOrder(pbCreate, pbCancel);

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
    } // retranslateUi

};

namespace Ui {
    class createkeyfile: public Ui_createkeyfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEKEYFILE_H
