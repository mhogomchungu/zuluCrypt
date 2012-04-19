/********************************************************************************
** Form generated from reading UI file 'backupluksheader.ui'
**
** Created: Thu Apr 19 00:34:55 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BACKUPLUKSHEADER_H
#define UI_BACKUPLUKSHEADER_H

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

class Ui_backupluksheader
{
public:
    QLineEdit *lineEditBackUpName;
    QLabel *label;
    QLineEdit *lineEditPath;
    QPushButton *pbOpenFolder;
    QPushButton *pbCreate;
    QPushButton *pbCancel;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *lineEditDevicePath;
    QPushButton *pushButtonFile;
    QPushButton *pushButtonPartition;

    void setupUi(QWidget *backupluksheader)
    {
        if (backupluksheader->objectName().isEmpty())
            backupluksheader->setObjectName(QString::fromUtf8("backupluksheader"));
        backupluksheader->setWindowModality(Qt::ApplicationModal);
        backupluksheader->resize(571, 150);
        lineEditBackUpName = new QLineEdit(backupluksheader);
        lineEditBackUpName->setObjectName(QString::fromUtf8("lineEditBackUpName"));
        lineEditBackUpName->setGeometry(QRect(130, 40, 311, 31));
        label = new QLabel(backupluksheader);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 40, 121, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditPath = new QLineEdit(backupluksheader);
        lineEditPath->setObjectName(QString::fromUtf8("lineEditPath"));
        lineEditPath->setGeometry(QRect(130, 70, 311, 31));
        pbOpenFolder = new QPushButton(backupluksheader);
        pbOpenFolder->setObjectName(QString::fromUtf8("pbOpenFolder"));
        pbOpenFolder->setGeometry(QRect(440, 40, 41, 31));
        pbCreate = new QPushButton(backupluksheader);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(190, 110, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCreate->setDefault(true);
        pbCancel = new QPushButton(backupluksheader);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(290, 110, 91, 31));
        pbCancel->setAutoDefault(false);
        pbCancel->setDefault(false);
        label_2 = new QLabel(backupluksheader);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 70, 131, 31));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_3 = new QLabel(backupluksheader);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 10, 131, 31));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditDevicePath = new QLineEdit(backupluksheader);
        lineEditDevicePath->setObjectName(QString::fromUtf8("lineEditDevicePath"));
        lineEditDevicePath->setGeometry(QRect(130, 10, 311, 31));
        pushButtonFile = new QPushButton(backupluksheader);
        pushButtonFile->setObjectName(QString::fromUtf8("pushButtonFile"));
        pushButtonFile->setGeometry(QRect(440, 10, 41, 31));
        pushButtonPartition = new QPushButton(backupluksheader);
        pushButtonPartition->setObjectName(QString::fromUtf8("pushButtonPartition"));
        pushButtonPartition->setGeometry(QRect(480, 10, 41, 31));
        QWidget::setTabOrder(lineEditBackUpName, lineEditPath);
        QWidget::setTabOrder(lineEditPath, pbOpenFolder);
        QWidget::setTabOrder(pbOpenFolder, pbCreate);
        QWidget::setTabOrder(pbCreate, pbCancel);

        retranslateUi(backupluksheader);

        QMetaObject::connectSlotsByName(backupluksheader);
    } // setupUi

    void retranslateUi(QWidget *backupluksheader)
    {
        backupluksheader->setWindowTitle(QApplication::translate("backupluksheader", "backup luks header", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("backupluksheader", "backup name: ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lineEditPath->setToolTip(QApplication::translate("backupluksheader", "path to a folder to create a key in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pbOpenFolder->setToolTip(QApplication::translate("backupluksheader", "open a folder a key file will be created in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbOpenFolder->setText(QString());
        pbCreate->setText(QApplication::translate("backupluksheader", "c&reate", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("backupluksheader", "&cancel", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("backupluksheader", "destination folder: ", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("backupluksheader", "device path: ", 0, QApplication::UnicodeUTF8));
        pushButtonFile->setText(QString());
        pushButtonPartition->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class backupluksheader: public Ui_backupluksheader {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BACKUPLUKSHEADER_H
