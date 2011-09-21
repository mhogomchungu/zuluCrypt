/********************************************************************************
** Form generated from reading UI file 'luksdeletekey.ui'
**
** Created: Wed Sep 21 13:48:59 2011
**      by: Qt User Interface Compiler version 4.7.4
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

    void setupUi(QDialog *luksdeletekey)
    {
        if (luksdeletekey->objectName().isEmpty())
            luksdeletekey->setObjectName(QString::fromUtf8("luksdeletekey"));
        luksdeletekey->setWindowModality(Qt::WindowModal);
        luksdeletekey->resize(452, 167);
        luksdeletekey->setModal(true);
        lineEditPassphrase = new QLineEdit(luksdeletekey);
        lineEditPassphrase->setObjectName(QString::fromUtf8("lineEditPassphrase"));
        lineEditPassphrase->setGeometry(QRect(100, 40, 291, 31));
        labelPassphrase = new QLabel(luksdeletekey);
        labelPassphrase->setObjectName(QString::fromUtf8("labelPassphrase"));
        labelPassphrase->setGeometry(QRect(20, 40, 81, 31));
        groupBox = new QGroupBox(luksdeletekey);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 70, 411, 61));
        rbPassphrase = new QRadioButton(groupBox);
        rbPassphrase->setObjectName(QString::fromUtf8("rbPassphrase"));
        rbPassphrase->setGeometry(QRect(70, 10, 311, 21));
        rbPassphraseFromFile = new QRadioButton(groupBox);
        rbPassphraseFromFile->setObjectName(QString::fromUtf8("rbPassphraseFromFile"));
        rbPassphraseFromFile->setGeometry(QRect(70, 30, 301, 21));
        pushButtonOpenKeyFile = new QPushButton(luksdeletekey);
        pushButtonOpenKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenKeyFile"));
        pushButtonOpenKeyFile->setGeometry(QRect(390, 40, 41, 31));
        pushButtonDelete = new QPushButton(luksdeletekey);
        pushButtonDelete->setObjectName(QString::fromUtf8("pushButtonDelete"));
        pushButtonDelete->setGeometry(QRect(130, 130, 91, 31));
        pushButtonCancel = new QPushButton(luksdeletekey);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(230, 130, 91, 31));
        lineEditVolumePath = new QLineEdit(luksdeletekey);
        lineEditVolumePath->setObjectName(QString::fromUtf8("lineEditVolumePath"));
        lineEditVolumePath->setGeometry(QRect(100, 10, 291, 31));
        label = new QLabel(luksdeletekey);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 15, 81, 21));
        pushButtonOpenVolume = new QPushButton(luksdeletekey);
        pushButtonOpenVolume->setObjectName(QString::fromUtf8("pushButtonOpenVolume"));
        pushButtonOpenVolume->setGeometry(QRect(390, 10, 41, 31));

        retranslateUi(luksdeletekey);

        QMetaObject::connectSlotsByName(luksdeletekey);
    } // setupUi

    void retranslateUi(QDialog *luksdeletekey)
    {
        luksdeletekey->setWindowTitle(QApplication::translate("luksdeletekey", "remove a key from a volume", 0, QApplication::UnicodeUTF8));
        labelPassphrase->setText(QApplication::translate("luksdeletekey", "passphrase", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        rbPassphrase->setText(QApplication::translate("luksdeletekey", "existing passphrase in a volume to delete", 0, QApplication::UnicodeUTF8));
        rbPassphraseFromFile->setText(QApplication::translate("luksdeletekey", "existing passphrase from key file to delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenKeyFile->setToolTip(QApplication::translate("luksdeletekey", "open a keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenKeyFile->setText(QString());
        pushButtonDelete->setText(QApplication::translate("luksdeletekey", "delete", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("luksdeletekey", "cancel", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("luksdeletekey", "volume path", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenVolume->setToolTip(QApplication::translate("luksdeletekey", "open an encrypted volume", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenVolume->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class luksdeletekey: public Ui_luksdeletekey {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LUKSDELETEKEY_H
