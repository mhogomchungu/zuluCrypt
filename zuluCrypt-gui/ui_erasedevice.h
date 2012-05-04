/********************************************************************************
** Form generated from reading UI file 'erasedevice.ui'
**
** Created: Fri May 4 16:57:26 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERASEDEVICE_H
#define UI_ERASEDEVICE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_erasedevice
{
public:
    QLabel *labelPath;
    QPushButton *pushButtonFile;
    QPushButton *pushButtonPartition;
    QLabel *label;
    QProgressBar *progressBar;
    QPushButton *pushButtonStart;
    QPushButton *pushButtonCancel;
    QLineEdit *lineEdit;

    void setupUi(QWidget *erasedevice)
    {
        if (erasedevice->objectName().isEmpty())
            erasedevice->setObjectName(QString::fromUtf8("erasedevice"));
        erasedevice->resize(532, 120);
        labelPath = new QLabel(erasedevice);
        labelPath->setObjectName(QString::fromUtf8("labelPath"));
        labelPath->setGeometry(QRect(0, 20, 111, 31));
        labelPath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonFile = new QPushButton(erasedevice);
        pushButtonFile->setObjectName(QString::fromUtf8("pushButtonFile"));
        pushButtonFile->setGeometry(QRect(410, 20, 31, 31));
        pushButtonPartition = new QPushButton(erasedevice);
        pushButtonPartition->setObjectName(QString::fromUtf8("pushButtonPartition"));
        pushButtonPartition->setGeometry(QRect(440, 20, 31, 31));
        label = new QLabel(erasedevice);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 40, 101, 41));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        progressBar = new QProgressBar(erasedevice);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(120, 50, 291, 21));
        progressBar->setValue(24);
        pushButtonStart = new QPushButton(erasedevice);
        pushButtonStart->setObjectName(QString::fromUtf8("pushButtonStart"));
        pushButtonStart->setGeometry(QRect(190, 80, 81, 31));
        pushButtonCancel = new QPushButton(erasedevice);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(270, 80, 81, 31));
        lineEdit = new QLineEdit(erasedevice);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(120, 20, 291, 31));

        retranslateUi(erasedevice);

        QMetaObject::connectSlotsByName(erasedevice);
    } // setupUi

    void retranslateUi(QWidget *erasedevice)
    {
        erasedevice->setWindowTitle(QApplication::translate("erasedevice", "erase data on the device by writing random data over them", 0, QApplication::UnicodeUTF8));
        labelPath->setText(QApplication::translate("erasedevice", "path to device", 0, QApplication::UnicodeUTF8));
        pushButtonFile->setText(QString());
        pushButtonPartition->setText(QString());
        label->setText(QApplication::translate("erasedevice", "% completed", 0, QApplication::UnicodeUTF8));
        pushButtonStart->setText(QApplication::translate("erasedevice", "&start", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("erasedevice", "&cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class erasedevice: public Ui_erasedevice {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERASEDEVICE_H
