/********************************************************************************
** Form generated from reading UI file 'createfile.ui'
**
** Created: Wed Nov 23 13:04:53 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEFILE_H
#define UI_CREATEFILE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_createfile
{
public:
    QLineEdit *lineEditFileName;
    QLabel *label;
    QLineEdit *lineEditFilePath;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *lineEditFileSize;
    QProgressBar *progressBar;
    QPushButton *pbOpenFolder;
    QPushButton *pbCancel;
    QLabel *label_4;
    QPushButton *pbCreate;
    QComboBox *comboBox;
    QComboBox *comboBoxRNG;
    QLabel *label_5;

    void setupUi(QWidget *createfile)
    {
        if (createfile->objectName().isEmpty())
            createfile->setObjectName(QString::fromUtf8("createfile"));
        createfile->resize(475, 196);
        lineEditFileName = new QLineEdit(createfile);
        lineEditFileName->setObjectName(QString::fromUtf8("lineEditFileName"));
        lineEditFileName->setGeometry(QRect(120, 10, 271, 31));
        label = new QLabel(createfile);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 10, 111, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFilePath = new QLineEdit(createfile);
        lineEditFilePath->setObjectName(QString::fromUtf8("lineEditFilePath"));
        lineEditFilePath->setGeometry(QRect(120, 40, 271, 31));
        label_2 = new QLabel(createfile);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 40, 111, 31));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_3 = new QLabel(createfile);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 70, 111, 31));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFileSize = new QLineEdit(createfile);
        lineEditFileSize->setObjectName(QString::fromUtf8("lineEditFileSize"));
        lineEditFileSize->setGeometry(QRect(120, 70, 113, 31));
        progressBar = new QProgressBar(createfile);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(120, 130, 271, 23));
        progressBar->setValue(24);
        pbOpenFolder = new QPushButton(createfile);
        pbOpenFolder->setObjectName(QString::fromUtf8("pbOpenFolder"));
        pbOpenFolder->setGeometry(QRect(390, 40, 31, 31));
        pbCancel = new QPushButton(createfile);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(260, 160, 91, 31));
        pbCancel->setAutoDefault(false);
        pbCancel->setDefault(false);
        label_4 = new QLabel(createfile);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 130, 101, 21));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pbCreate = new QPushButton(createfile);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(160, 160, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCreate->setDefault(true);
        comboBox = new QComboBox(createfile);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(230, 70, 51, 31));
        comboBox->setFrame(true);
        comboBoxRNG = new QComboBox(createfile);
        comboBoxRNG->insertItems(0, QStringList()
         << QString::fromUtf8("/dev/urandom")
         << QString::fromUtf8("/dev/zero")
         << QString::fromUtf8("/dev/random")
        );
        comboBoxRNG->setObjectName(QString::fromUtf8("comboBoxRNG"));
        comboBoxRNG->setGeometry(QRect(120, 100, 161, 31));
        label_5 = new QLabel(createfile);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(50, 104, 55, 20));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        QWidget::setTabOrder(lineEditFileName, lineEditFilePath);
        QWidget::setTabOrder(lineEditFilePath, pbOpenFolder);
        QWidget::setTabOrder(pbOpenFolder, lineEditFileSize);
        QWidget::setTabOrder(lineEditFileSize, comboBox);
        QWidget::setTabOrder(comboBox, comboBoxRNG);
        QWidget::setTabOrder(comboBoxRNG, pbCreate);
        QWidget::setTabOrder(pbCreate, pbCancel);

        retranslateUi(createfile);

        QMetaObject::connectSlotsByName(createfile);
    } // setupUi

    void retranslateUi(QWidget *createfile)
    {
        createfile->setWindowTitle(QApplication::translate("createfile", "create a container file", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("createfile", "file name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("createfile", "file path", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("createfile", "file size", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pbOpenFolder->setToolTip(QApplication::translate("createfile", "open a folder dialog box", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbOpenFolder->setText(QString());
        pbCancel->setText(QApplication::translate("createfile", "&cancel", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("createfile", "% complete", 0, QApplication::UnicodeUTF8));
        pbCreate->setText(QApplication::translate("createfile", "c&reate", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("createfile", "KB", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("createfile", "MB", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("createfile", "GB", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBoxRNG->setToolTip(QApplication::translate("createfile", "/dev/random is slowest but most secure.\n"
"/dev/zero is fastest but least secure.\n"
"/dev/urandom is in the middle and secure enough.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_5->setText(QApplication::translate("createfile", "rng", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class createfile: public Ui_createfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEFILE_H
