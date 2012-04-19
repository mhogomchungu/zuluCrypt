/********************************************************************************
** Form generated from reading UI file 'manageluksheader.ui'
**
** Created: Thu Apr 19 04:23:20 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANAGELUKSHEADER_H
#define UI_MANAGELUKSHEADER_H

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

class Ui_manageluksheader
{
public:
    QLineEdit *lineEditBackUpName;
    QLabel *labelBackUpHeader;
    QPushButton *pbOpenFolder;
    QPushButton *pbCreate;
    QPushButton *pbCancel;
    QLabel *labelDevicePath;
    QLineEdit *lineEditDevicePath;
    QPushButton *pushButtonFile;
    QPushButton *pushButtonPartition;

    void setupUi(QWidget *manageluksheader)
    {
        if (manageluksheader->objectName().isEmpty())
            manageluksheader->setObjectName(QString::fromUtf8("manageluksheader"));
        manageluksheader->setWindowModality(Qt::ApplicationModal);
        manageluksheader->resize(571, 119);
        lineEditBackUpName = new QLineEdit(manageluksheader);
        lineEditBackUpName->setObjectName(QString::fromUtf8("lineEditBackUpName"));
        lineEditBackUpName->setGeometry(QRect(130, 40, 311, 31));
        labelBackUpHeader = new QLabel(manageluksheader);
        labelBackUpHeader->setObjectName(QString::fromUtf8("labelBackUpHeader"));
        labelBackUpHeader->setGeometry(QRect(10, 40, 121, 31));
        labelBackUpHeader->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pbOpenFolder = new QPushButton(manageluksheader);
        pbOpenFolder->setObjectName(QString::fromUtf8("pbOpenFolder"));
        pbOpenFolder->setGeometry(QRect(440, 40, 41, 31));
        pbCreate = new QPushButton(manageluksheader);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(190, 80, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCreate->setDefault(true);
        pbCancel = new QPushButton(manageluksheader);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(290, 80, 91, 31));
        pbCancel->setAutoDefault(false);
        pbCancel->setDefault(false);
        labelDevicePath = new QLabel(manageluksheader);
        labelDevicePath->setObjectName(QString::fromUtf8("labelDevicePath"));
        labelDevicePath->setGeometry(QRect(0, 10, 131, 31));
        labelDevicePath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditDevicePath = new QLineEdit(manageluksheader);
        lineEditDevicePath->setObjectName(QString::fromUtf8("lineEditDevicePath"));
        lineEditDevicePath->setGeometry(QRect(130, 10, 311, 31));
        pushButtonFile = new QPushButton(manageluksheader);
        pushButtonFile->setObjectName(QString::fromUtf8("pushButtonFile"));
        pushButtonFile->setGeometry(QRect(440, 10, 41, 31));
        pushButtonPartition = new QPushButton(manageluksheader);
        pushButtonPartition->setObjectName(QString::fromUtf8("pushButtonPartition"));
        pushButtonPartition->setGeometry(QRect(480, 10, 41, 31));
        QWidget::setTabOrder(lineEditBackUpName, pbOpenFolder);
        QWidget::setTabOrder(pbOpenFolder, pbCreate);
        QWidget::setTabOrder(pbCreate, pbCancel);

        retranslateUi(manageluksheader);

        QMetaObject::connectSlotsByName(manageluksheader);
    } // setupUi

    void retranslateUi(QWidget *manageluksheader)
    {
        manageluksheader->setWindowTitle(QApplication::translate("manageluksheader", "backup luks header", 0, QApplication::UnicodeUTF8));
        labelBackUpHeader->setText(QApplication::translate("manageluksheader", "backup name: ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pbOpenFolder->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        pbOpenFolder->setText(QString());
        pbCreate->setText(QApplication::translate("manageluksheader", "c&reate", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("manageluksheader", "&cancel", 0, QApplication::UnicodeUTF8));
        labelDevicePath->setText(QApplication::translate("manageluksheader", "device path: ", 0, QApplication::UnicodeUTF8));
        pushButtonFile->setText(QString());
        pushButtonPartition->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class manageluksheader: public Ui_manageluksheader {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANAGELUKSHEADER_H
