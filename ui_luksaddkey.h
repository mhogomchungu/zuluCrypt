/********************************************************************************
** Form generated from reading UI file 'luksaddkey.ui'
**
** Created: Tue Sep 20 20:54:44 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LUKSADDKEY_H
#define UI_LUKSADDKEY_H

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

class Ui_luksaddkeyUI
{
public:
    QLabel *labelLuksVolume;
    QPushButton *pushButtonOpenFile;
    QPushButton *pushButtonOpenPartition;
    QGroupBox *groupBox;
    QRadioButton *radioButtonPassphraseinVolume;
    QRadioButton *radioButtonPassphraseInVolumeFromFile;
    QLabel *labelExistingPassphrase;
    QPushButton *pushButtonOpenExistingKeyFile;
    QLineEdit *textEditExistingPassphrase;
    QGroupBox *groupBox_2;
    QRadioButton *radioButtonNewPassphrase;
    QRadioButton *radioButtonNewPassphraseFromFile;
    QPushButton *pushButtonOpenNewKeyFile;
    QLabel *labelNewPassphrase;
    QLineEdit *textEditPassphraseToAdd;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonCancel;
    QLineEdit *textEditPathToVolume;

    void setupUi(QDialog *luksaddkeyUI)
    {
        if (luksaddkeyUI->objectName().isEmpty())
            luksaddkeyUI->setObjectName(QString::fromUtf8("luksaddkeyUI"));
        luksaddkeyUI->resize(528, 313);
        luksaddkeyUI->setModal(true);
        labelLuksVolume = new QLabel(luksaddkeyUI);
        labelLuksVolume->setObjectName(QString::fromUtf8("labelLuksVolume"));
        labelLuksVolume->setGeometry(QRect(10, 10, 121, 31));
        pushButtonOpenFile = new QPushButton(luksaddkeyUI);
        pushButtonOpenFile->setObjectName(QString::fromUtf8("pushButtonOpenFile"));
        pushButtonOpenFile->setGeometry(QRect(460, 10, 31, 31));
        pushButtonOpenPartition = new QPushButton(luksaddkeyUI);
        pushButtonOpenPartition->setObjectName(QString::fromUtf8("pushButtonOpenPartition"));
        pushButtonOpenPartition->setGeometry(QRect(490, 10, 31, 31));
        groupBox = new QGroupBox(luksaddkeyUI);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 40, 511, 131));
        radioButtonPassphraseinVolume = new QRadioButton(groupBox);
        radioButtonPassphraseinVolume->setObjectName(QString::fromUtf8("radioButtonPassphraseinVolume"));
        radioButtonPassphraseinVolume->setGeometry(QRect(130, 30, 181, 21));
        radioButtonPassphraseInVolumeFromFile = new QRadioButton(groupBox);
        radioButtonPassphraseInVolumeFromFile->setObjectName(QString::fromUtf8("radioButtonPassphraseInVolumeFromFile"));
        radioButtonPassphraseInVolumeFromFile->setGeometry(QRect(130, 50, 221, 21));
        labelExistingPassphrase = new QLabel(groupBox);
        labelExistingPassphrase->setObjectName(QString::fromUtf8("labelExistingPassphrase"));
        labelExistingPassphrase->setGeometry(QRect(50, 80, 71, 31));
        pushButtonOpenExistingKeyFile = new QPushButton(groupBox);
        pushButtonOpenExistingKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenExistingKeyFile"));
        pushButtonOpenExistingKeyFile->setGeometry(QRect(450, 80, 31, 31));
        textEditExistingPassphrase = new QLineEdit(groupBox);
        textEditExistingPassphrase->setObjectName(QString::fromUtf8("textEditExistingPassphrase"));
        textEditExistingPassphrase->setGeometry(QRect(120, 80, 331, 31));
        groupBox_2 = new QGroupBox(luksaddkeyUI);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 160, 511, 131));
        radioButtonNewPassphrase = new QRadioButton(groupBox_2);
        radioButtonNewPassphrase->setObjectName(QString::fromUtf8("radioButtonNewPassphrase"));
        radioButtonNewPassphrase->setGeometry(QRect(130, 30, 321, 21));
        radioButtonNewPassphraseFromFile = new QRadioButton(groupBox_2);
        radioButtonNewPassphraseFromFile->setObjectName(QString::fromUtf8("radioButtonNewPassphraseFromFile"));
        radioButtonNewPassphraseFromFile->setGeometry(QRect(130, 50, 361, 21));
        pushButtonOpenNewKeyFile = new QPushButton(groupBox_2);
        pushButtonOpenNewKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenNewKeyFile"));
        pushButtonOpenNewKeyFile->setGeometry(QRect(450, 80, 31, 31));
        labelNewPassphrase = new QLabel(groupBox_2);
        labelNewPassphrase->setObjectName(QString::fromUtf8("labelNewPassphrase"));
        labelNewPassphrase->setGeometry(QRect(50, 80, 71, 31));
        textEditPassphraseToAdd = new QLineEdit(groupBox_2);
        textEditPassphraseToAdd->setObjectName(QString::fromUtf8("textEditPassphraseToAdd"));
        textEditPassphraseToAdd->setGeometry(QRect(120, 80, 331, 31));
        pushButtonAdd = new QPushButton(luksaddkeyUI);
        pushButtonAdd->setObjectName(QString::fromUtf8("pushButtonAdd"));
        pushButtonAdd->setGeometry(QRect(170, 280, 91, 31));
        pushButtonCancel = new QPushButton(luksaddkeyUI);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(270, 280, 91, 31));
        textEditPathToVolume = new QLineEdit(luksaddkeyUI);
        textEditPathToVolume->setObjectName(QString::fromUtf8("textEditPathToVolume"));
        textEditPathToVolume->setGeometry(QRect(130, 10, 331, 31));

        retranslateUi(luksaddkeyUI);

        QMetaObject::connectSlotsByName(luksaddkeyUI);
    } // setupUi

    void retranslateUi(QDialog *luksaddkeyUI)
    {
        luksaddkeyUI->setWindowTitle(QApplication::translate("luksaddkeyUI", "add a key to a luks volume", 0, QApplication::UnicodeUTF8));
        labelLuksVolume->setText(QApplication::translate("luksaddkeyUI", "Path to luks volume", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenFile->setToolTip(QApplication::translate("luksaddkeyUI", "open file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenFile->setText(QString());
#ifndef QT_NO_TOOLTIP
        pushButtonOpenPartition->setToolTip(QApplication::translate("luksaddkeyUI", "open partition", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenPartition->setText(QString());
        groupBox->setTitle(QApplication::translate("luksaddkeyUI", "passphrase already in the volume", 0, QApplication::UnicodeUTF8));
        radioButtonPassphraseinVolume->setText(QApplication::translate("luksaddkeyUI", "passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonPassphraseInVolumeFromFile->setText(QApplication::translate("luksaddkeyUI", "passphrase from a keyfile", 0, QApplication::UnicodeUTF8));
        labelExistingPassphrase->setText(QApplication::translate("luksaddkeyUI", "Passphrase", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenExistingKeyFile->setToolTip(QApplication::translate("luksaddkeyUI", "open keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenExistingKeyFile->setText(QString());
        groupBox_2->setTitle(QApplication::translate("luksaddkeyUI", "        passphrase to be added to the volume", 0, QApplication::UnicodeUTF8));
        radioButtonNewPassphrase->setText(QApplication::translate("luksaddkeyUI", "new passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonNewPassphraseFromFile->setText(QApplication::translate("luksaddkeyUI", "new passphrase from a key file", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenNewKeyFile->setToolTip(QApplication::translate("luksaddkeyUI", "open keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenNewKeyFile->setText(QString());
        labelNewPassphrase->setText(QApplication::translate("luksaddkeyUI", "Passphrase", 0, QApplication::UnicodeUTF8));
        pushButtonAdd->setText(QApplication::translate("luksaddkeyUI", "Add", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("luksaddkeyUI", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class luksaddkeyUI: public Ui_luksaddkeyUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LUKSADDKEY_H
