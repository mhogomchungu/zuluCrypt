/********************************************************************************
** Form generated from reading UI file 'luksdeletekey.ui'
**
** Created: Fri May 18 11:01:47 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LUKSDELETEKEY_H
#define UI_LUKSDELETEKEY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_luksdeletekey
{
public:
    QLineEdit *lineEditPassphrase;
    QLabel *labelPassphrase;
    QGroupBox *groupBox;
    QRadioButton *rbPassphrase;
    QRadioButton *rbPassphraseFromFile;
    QPushButton *pushButtonOpenKeyFile;
    QPushButton *pushButtonDelete;
    QPushButton *pushButtonCancel;
    QLineEdit *lineEditVolumePath;
    QLabel *label;
    QPushButton *pushButtonOpenVolume;
    QPushButton *pushButtonOpenPartition;

    void setupUi(QDialog *luksdeletekey)
    {
        if (luksdeletekey->objectName().isEmpty())
            luksdeletekey->setObjectName(QString::fromUtf8("luksdeletekey"));
        luksdeletekey->setWindowModality(Qt::ApplicationModal);
        luksdeletekey->resize(542, 167);
        luksdeletekey->setModal(true);
        lineEditPassphrase = new QLineEdit(luksdeletekey);
        lineEditPassphrase->setObjectName(QString::fromUtf8("lineEditPassphrase"));
        lineEditPassphrase->setGeometry(QRect(110, 40, 321, 31));
        labelPassphrase = new QLabel(luksdeletekey);
        labelPassphrase->setObjectName(QString::fromUtf8("labelPassphrase"));
        labelPassphrase->setGeometry(QRect(0, 40, 101, 31));
        labelPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        groupBox = new QGroupBox(luksdeletekey);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(110, 70, 321, 61));
        rbPassphrase = new QRadioButton(groupBox);
        rbPassphrase->setObjectName(QString::fromUtf8("rbPassphrase"));
        rbPassphrase->setGeometry(QRect(10, 0, 311, 31));
        rbPassphraseFromFile = new QRadioButton(groupBox);
        rbPassphraseFromFile->setObjectName(QString::fromUtf8("rbPassphraseFromFile"));
        rbPassphraseFromFile->setGeometry(QRect(10, 20, 311, 31));
        pushButtonOpenKeyFile = new QPushButton(luksdeletekey);
        pushButtonOpenKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenKeyFile"));
        pushButtonOpenKeyFile->setGeometry(QRect(430, 40, 31, 31));
        pushButtonOpenKeyFile->setAutoDefault(false);
        pushButtonDelete = new QPushButton(luksdeletekey);
        pushButtonDelete->setObjectName(QString::fromUtf8("pushButtonDelete"));
        pushButtonDelete->setGeometry(QRect(180, 130, 91, 31));
        pushButtonDelete->setAutoDefault(false);
        pushButtonCancel = new QPushButton(luksdeletekey);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(270, 130, 91, 31));
        pushButtonCancel->setAutoDefault(false);
        pushButtonCancel->setDefault(true);
        lineEditVolumePath = new QLineEdit(luksdeletekey);
        lineEditVolumePath->setObjectName(QString::fromUtf8("lineEditVolumePath"));
        lineEditVolumePath->setGeometry(QRect(110, 10, 321, 31));
        label = new QLabel(luksdeletekey);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 10, 101, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonOpenVolume = new QPushButton(luksdeletekey);
        pushButtonOpenVolume->setObjectName(QString::fromUtf8("pushButtonOpenVolume"));
        pushButtonOpenVolume->setGeometry(QRect(430, 10, 31, 31));
        pushButtonOpenVolume->setAutoDefault(false);
        pushButtonOpenPartition = new QPushButton(luksdeletekey);
        pushButtonOpenPartition->setObjectName(QString::fromUtf8("pushButtonOpenPartition"));
        pushButtonOpenPartition->setGeometry(QRect(460, 10, 31, 31));
        pushButtonOpenPartition->setAutoDefault(false);
        QWidget::setTabOrder(lineEditVolumePath, pushButtonOpenVolume);
        QWidget::setTabOrder(pushButtonOpenVolume, pushButtonOpenPartition);
        QWidget::setTabOrder(pushButtonOpenPartition, lineEditPassphrase);
        QWidget::setTabOrder(lineEditPassphrase, pushButtonOpenKeyFile);
        QWidget::setTabOrder(pushButtonOpenKeyFile, rbPassphrase);
        QWidget::setTabOrder(rbPassphrase, rbPassphraseFromFile);
        QWidget::setTabOrder(rbPassphraseFromFile, pushButtonDelete);
        QWidget::setTabOrder(pushButtonDelete, pushButtonCancel);

        retranslateUi(luksdeletekey);

        QMetaObject::connectSlotsByName(luksdeletekey);
    } // setupUi

    void retranslateUi(QDialog *luksdeletekey)
    {
        luksdeletekey->setWindowTitle(QApplication::translate("luksdeletekey", "remove a key from a volume", 0, QApplication::UnicodeUTF8));
        labelPassphrase->setText(QApplication::translate("luksdeletekey", "key ", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        rbPassphrase->setText(QApplication::translate("luksdeletekey", "existing key in the volume to delete", 0, QApplication::UnicodeUTF8));
        rbPassphraseFromFile->setText(QApplication::translate("luksdeletekey", "existing key from a keyfile to delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenKeyFile->setToolTip(QApplication::translate("luksdeletekey", "open a keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenKeyFile->setText(QString());
        pushButtonDelete->setText(QApplication::translate("luksdeletekey", "&delete", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("luksdeletekey", "&cancel", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("luksdeletekey", "volume path", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenVolume->setToolTip(QApplication::translate("luksdeletekey", "open an encrypted file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenVolume->setText(QString());
#ifndef QT_NO_TOOLTIP
        pushButtonOpenPartition->setToolTip(QApplication::translate("luksdeletekey", "open an encrypted partition", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenPartition->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class luksdeletekey: public Ui_luksdeletekey {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LUKSDELETEKEY_H
