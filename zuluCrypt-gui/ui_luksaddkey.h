/********************************************************************************
** Form generated from reading UI file 'luksaddkey.ui'
**
** Created: Tue Dec 6 22:41:23 2011
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
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonCancel;
    QLineEdit *textEditPathToVolume;
    QGroupBox *groupBox_2;
    QRadioButton *radioButtonNewPassphrase;
    QRadioButton *radioButtonNewPassphraseFromFile;
    QPushButton *pushButtonOpenNewKeyFile;
    QLabel *labelNewPassphrase;
    QLineEdit *textEditPassphraseToAdd;
    QLineEdit *lineEditReEnterPassphrase;
    QLabel *labelReEnterPassphrase;

    void setupUi(QDialog *luksaddkeyUI)
    {
        if (luksaddkeyUI->objectName().isEmpty())
            luksaddkeyUI->setObjectName(QString::fromUtf8("luksaddkeyUI"));
        luksaddkeyUI->setWindowModality(Qt::ApplicationModal);
        luksaddkeyUI->resize(528, 295);
        luksaddkeyUI->setModal(true);
        labelLuksVolume = new QLabel(luksaddkeyUI);
        labelLuksVolume->setObjectName(QString::fromUtf8("labelLuksVolume"));
        labelLuksVolume->setGeometry(QRect(10, 10, 121, 31));
        labelLuksVolume->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonOpenFile = new QPushButton(luksaddkeyUI);
        pushButtonOpenFile->setObjectName(QString::fromUtf8("pushButtonOpenFile"));
        pushButtonOpenFile->setGeometry(QRect(460, 10, 31, 31));
        pushButtonOpenPartition = new QPushButton(luksaddkeyUI);
        pushButtonOpenPartition->setObjectName(QString::fromUtf8("pushButtonOpenPartition"));
        pushButtonOpenPartition->setGeometry(QRect(490, 10, 31, 31));
        groupBox = new QGroupBox(luksaddkeyUI);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 40, 511, 101));
        radioButtonPassphraseinVolume = new QRadioButton(groupBox);
        radioButtonPassphraseinVolume->setObjectName(QString::fromUtf8("radioButtonPassphraseinVolume"));
        radioButtonPassphraseinVolume->setGeometry(QRect(150, 20, 181, 21));
        radioButtonPassphraseInVolumeFromFile = new QRadioButton(groupBox);
        radioButtonPassphraseInVolumeFromFile->setObjectName(QString::fromUtf8("radioButtonPassphraseInVolumeFromFile"));
        radioButtonPassphraseInVolumeFromFile->setGeometry(QRect(150, 40, 221, 21));
        labelExistingPassphrase = new QLabel(groupBox);
        labelExistingPassphrase->setObjectName(QString::fromUtf8("labelExistingPassphrase"));
        labelExistingPassphrase->setGeometry(QRect(0, 60, 121, 31));
        labelExistingPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonOpenExistingKeyFile = new QPushButton(groupBox);
        pushButtonOpenExistingKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenExistingKeyFile"));
        pushButtonOpenExistingKeyFile->setGeometry(QRect(450, 60, 31, 31));
        pushButtonOpenExistingKeyFile->setAutoDefault(false);
        textEditExistingPassphrase = new QLineEdit(groupBox);
        textEditExistingPassphrase->setObjectName(QString::fromUtf8("textEditExistingPassphrase"));
        textEditExistingPassphrase->setGeometry(QRect(130, 60, 321, 31));
        pushButtonAdd = new QPushButton(luksaddkeyUI);
        pushButtonAdd->setObjectName(QString::fromUtf8("pushButtonAdd"));
        pushButtonAdd->setGeometry(QRect(170, 260, 91, 31));
        pushButtonAdd->setAutoDefault(false);
        pushButtonAdd->setDefault(true);
        pushButtonCancel = new QPushButton(luksaddkeyUI);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(260, 260, 91, 31));
        pushButtonCancel->setAutoDefault(false);
        pushButtonCancel->setDefault(false);
        textEditPathToVolume = new QLineEdit(luksaddkeyUI);
        textEditPathToVolume->setObjectName(QString::fromUtf8("textEditPathToVolume"));
        textEditPathToVolume->setGeometry(QRect(140, 10, 321, 31));
        groupBox_2 = new QGroupBox(luksaddkeyUI);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 140, 511, 131));
        radioButtonNewPassphrase = new QRadioButton(groupBox_2);
        radioButtonNewPassphrase->setObjectName(QString::fromUtf8("radioButtonNewPassphrase"));
        radioButtonNewPassphrase->setGeometry(QRect(150, 20, 321, 21));
        radioButtonNewPassphraseFromFile = new QRadioButton(groupBox_2);
        radioButtonNewPassphraseFromFile->setObjectName(QString::fromUtf8("radioButtonNewPassphraseFromFile"));
        radioButtonNewPassphraseFromFile->setGeometry(QRect(150, 40, 361, 21));
        pushButtonOpenNewKeyFile = new QPushButton(groupBox_2);
        pushButtonOpenNewKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenNewKeyFile"));
        pushButtonOpenNewKeyFile->setGeometry(QRect(450, 60, 31, 31));
        pushButtonOpenNewKeyFile->setAutoDefault(false);
        labelNewPassphrase = new QLabel(groupBox_2);
        labelNewPassphrase->setObjectName(QString::fromUtf8("labelNewPassphrase"));
        labelNewPassphrase->setGeometry(QRect(0, 60, 121, 31));
        labelNewPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textEditPassphraseToAdd = new QLineEdit(groupBox_2);
        textEditPassphraseToAdd->setObjectName(QString::fromUtf8("textEditPassphraseToAdd"));
        textEditPassphraseToAdd->setGeometry(QRect(130, 60, 321, 31));
        lineEditReEnterPassphrase = new QLineEdit(groupBox_2);
        lineEditReEnterPassphrase->setObjectName(QString::fromUtf8("lineEditReEnterPassphrase"));
        lineEditReEnterPassphrase->setGeometry(QRect(130, 90, 321, 31));
        labelReEnterPassphrase = new QLabel(groupBox_2);
        labelReEnterPassphrase->setObjectName(QString::fromUtf8("labelReEnterPassphrase"));
        labelReEnterPassphrase->setGeometry(QRect(0, 90, 121, 31));
        labelReEnterPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        QWidget::setTabOrder(textEditPathToVolume, pushButtonOpenFile);
        QWidget::setTabOrder(pushButtonOpenFile, pushButtonOpenPartition);
        QWidget::setTabOrder(pushButtonOpenPartition, radioButtonPassphraseinVolume);
        QWidget::setTabOrder(radioButtonPassphraseinVolume, radioButtonPassphraseInVolumeFromFile);
        QWidget::setTabOrder(radioButtonPassphraseInVolumeFromFile, textEditExistingPassphrase);
        QWidget::setTabOrder(textEditExistingPassphrase, pushButtonOpenExistingKeyFile);
        QWidget::setTabOrder(pushButtonOpenExistingKeyFile, radioButtonNewPassphrase);
        QWidget::setTabOrder(radioButtonNewPassphrase, radioButtonNewPassphraseFromFile);
        QWidget::setTabOrder(radioButtonNewPassphraseFromFile, textEditPassphraseToAdd);
        QWidget::setTabOrder(textEditPassphraseToAdd, pushButtonOpenNewKeyFile);
        QWidget::setTabOrder(pushButtonOpenNewKeyFile, lineEditReEnterPassphrase);
        QWidget::setTabOrder(lineEditReEnterPassphrase, pushButtonAdd);
        QWidget::setTabOrder(pushButtonAdd, pushButtonCancel);

        retranslateUi(luksaddkeyUI);

        QMetaObject::connectSlotsByName(luksaddkeyUI);
    } // setupUi

    void retranslateUi(QDialog *luksaddkeyUI)
    {
        luksaddkeyUI->setWindowTitle(QApplication::translate("luksaddkeyUI", "add a key to a luks volume", 0, QApplication::UnicodeUTF8));
        labelLuksVolume->setText(QApplication::translate("luksaddkeyUI", "path to luks volume", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenFile->setToolTip(QApplication::translate("luksaddkeyUI", "open file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenFile->setText(QString());
#ifndef QT_NO_TOOLTIP
        pushButtonOpenPartition->setToolTip(QApplication::translate("luksaddkeyUI", "open partition", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenPartition->setText(QString());
        groupBox->setTitle(QApplication::translate("luksaddkeyUI", "key already in the encrypted volume", 0, QApplication::UnicodeUTF8));
        radioButtonPassphraseinVolume->setText(QApplication::translate("luksaddkeyUI", "passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonPassphraseInVolumeFromFile->setText(QApplication::translate("luksaddkeyUI", "passphrase from a key file", 0, QApplication::UnicodeUTF8));
        labelExistingPassphrase->setText(QApplication::translate("luksaddkeyUI", "passphrase", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenExistingKeyFile->setToolTip(QApplication::translate("luksaddkeyUI", "open keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenExistingKeyFile->setText(QString());
        pushButtonAdd->setText(QApplication::translate("luksaddkeyUI", "&add", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("luksaddkeyUI", "&cancel", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("luksaddkeyUI", "         key to be added to the encrypted volume", 0, QApplication::UnicodeUTF8));
        radioButtonNewPassphrase->setText(QApplication::translate("luksaddkeyUI", "passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonNewPassphraseFromFile->setText(QApplication::translate("luksaddkeyUI", "passphrase from a key file", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenNewKeyFile->setToolTip(QApplication::translate("luksaddkeyUI", "open keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenNewKeyFile->setText(QString());
        labelNewPassphrase->setText(QApplication::translate("luksaddkeyUI", "passphrase", 0, QApplication::UnicodeUTF8));
        labelReEnterPassphrase->setText(QApplication::translate("luksaddkeyUI", "reenter passphrase", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class luksaddkeyUI: public Ui_luksaddkeyUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LUKSADDKEY_H
