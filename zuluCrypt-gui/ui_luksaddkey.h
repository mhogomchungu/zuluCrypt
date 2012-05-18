/********************************************************************************
** Form generated from reading UI file 'luksaddkey.ui'
**
** Created: Fri May 18 09:47:04 2012
**      by: Qt User Interface Compiler version 4.8.0
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

class Ui_luksaddkey
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

    void setupUi(QDialog *luksaddkey)
    {
        if (luksaddkey->objectName().isEmpty())
            luksaddkey->setObjectName(QString::fromUtf8("luksaddkey"));
        luksaddkey->setWindowModality(Qt::ApplicationModal);
        luksaddkey->resize(602, 327);
        luksaddkey->setModal(true);
        labelLuksVolume = new QLabel(luksaddkey);
        labelLuksVolume->setObjectName(QString::fromUtf8("labelLuksVolume"));
        labelLuksVolume->setGeometry(QRect(0, 10, 131, 31));
        labelLuksVolume->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonOpenFile = new QPushButton(luksaddkey);
        pushButtonOpenFile->setObjectName(QString::fromUtf8("pushButtonOpenFile"));
        pushButtonOpenFile->setGeometry(QRect(460, 10, 31, 31));
        pushButtonOpenPartition = new QPushButton(luksaddkey);
        pushButtonOpenPartition->setObjectName(QString::fromUtf8("pushButtonOpenPartition"));
        pushButtonOpenPartition->setGeometry(QRect(490, 10, 31, 31));
        groupBox = new QGroupBox(luksaddkey);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 40, 581, 111));
        groupBox->setAlignment(Qt::AlignCenter);
        radioButtonPassphraseinVolume = new QRadioButton(groupBox);
        radioButtonPassphraseinVolume->setObjectName(QString::fromUtf8("radioButtonPassphraseinVolume"));
        radioButtonPassphraseinVolume->setGeometry(QRect(180, 20, 271, 31));
        radioButtonPassphraseInVolumeFromFile = new QRadioButton(groupBox);
        radioButtonPassphraseInVolumeFromFile->setObjectName(QString::fromUtf8("radioButtonPassphraseInVolumeFromFile"));
        radioButtonPassphraseInVolumeFromFile->setGeometry(QRect(180, 40, 271, 31));
        labelExistingPassphrase = new QLabel(groupBox);
        labelExistingPassphrase->setObjectName(QString::fromUtf8("labelExistingPassphrase"));
        labelExistingPassphrase->setGeometry(QRect(0, 70, 121, 31));
        labelExistingPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonOpenExistingKeyFile = new QPushButton(groupBox);
        pushButtonOpenExistingKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenExistingKeyFile"));
        pushButtonOpenExistingKeyFile->setGeometry(QRect(450, 70, 31, 31));
        pushButtonOpenExistingKeyFile->setAutoDefault(false);
        textEditExistingPassphrase = new QLineEdit(groupBox);
        textEditExistingPassphrase->setObjectName(QString::fromUtf8("textEditExistingPassphrase"));
        textEditExistingPassphrase->setGeometry(QRect(130, 70, 321, 31));
        pushButtonAdd = new QPushButton(luksaddkey);
        pushButtonAdd->setObjectName(QString::fromUtf8("pushButtonAdd"));
        pushButtonAdd->setGeometry(QRect(210, 290, 91, 31));
        pushButtonAdd->setAutoDefault(false);
        pushButtonAdd->setDefault(true);
        pushButtonCancel = new QPushButton(luksaddkey);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(300, 290, 91, 31));
        pushButtonCancel->setAutoDefault(false);
        pushButtonCancel->setDefault(false);
        textEditPathToVolume = new QLineEdit(luksaddkey);
        textEditPathToVolume->setObjectName(QString::fromUtf8("textEditPathToVolume"));
        textEditPathToVolume->setGeometry(QRect(140, 10, 321, 31));
        groupBox_2 = new QGroupBox(luksaddkey);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 150, 581, 131));
        groupBox_2->setAlignment(Qt::AlignCenter);
        radioButtonNewPassphrase = new QRadioButton(groupBox_2);
        radioButtonNewPassphrase->setObjectName(QString::fromUtf8("radioButtonNewPassphrase"));
        radioButtonNewPassphrase->setGeometry(QRect(180, 20, 271, 31));
        radioButtonNewPassphraseFromFile = new QRadioButton(groupBox_2);
        radioButtonNewPassphraseFromFile->setObjectName(QString::fromUtf8("radioButtonNewPassphraseFromFile"));
        radioButtonNewPassphraseFromFile->setGeometry(QRect(180, 40, 271, 31));
        pushButtonOpenNewKeyFile = new QPushButton(groupBox_2);
        pushButtonOpenNewKeyFile->setObjectName(QString::fromUtf8("pushButtonOpenNewKeyFile"));
        pushButtonOpenNewKeyFile->setGeometry(QRect(450, 70, 31, 31));
        pushButtonOpenNewKeyFile->setAutoDefault(false);
        labelNewPassphrase = new QLabel(groupBox_2);
        labelNewPassphrase->setObjectName(QString::fromUtf8("labelNewPassphrase"));
        labelNewPassphrase->setGeometry(QRect(0, 70, 121, 31));
        labelNewPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textEditPassphraseToAdd = new QLineEdit(groupBox_2);
        textEditPassphraseToAdd->setObjectName(QString::fromUtf8("textEditPassphraseToAdd"));
        textEditPassphraseToAdd->setGeometry(QRect(130, 70, 321, 31));
        lineEditReEnterPassphrase = new QLineEdit(groupBox_2);
        lineEditReEnterPassphrase->setObjectName(QString::fromUtf8("lineEditReEnterPassphrase"));
        lineEditReEnterPassphrase->setGeometry(QRect(130, 100, 321, 31));
        labelReEnterPassphrase = new QLabel(groupBox_2);
        labelReEnterPassphrase->setObjectName(QString::fromUtf8("labelReEnterPassphrase"));
        labelReEnterPassphrase->setGeometry(QRect(0, 100, 121, 31));
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

        retranslateUi(luksaddkey);

        QMetaObject::connectSlotsByName(luksaddkey);
    } // setupUi

    void retranslateUi(QDialog *luksaddkey)
    {
        luksaddkey->setWindowTitle(QApplication::translate("luksaddkey", "add a key to a luks volume", 0, QApplication::UnicodeUTF8));
        labelLuksVolume->setText(QApplication::translate("luksaddkey", "volume path", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenFile->setToolTip(QApplication::translate("luksaddkey", "open file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenFile->setText(QString());
#ifndef QT_NO_TOOLTIP
        pushButtonOpenPartition->setToolTip(QApplication::translate("luksaddkey", "open partition", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenPartition->setText(QString());
        groupBox->setTitle(QApplication::translate("luksaddkey", "key already in the encrypted volume", 0, QApplication::UnicodeUTF8));
        radioButtonPassphraseinVolume->setText(QApplication::translate("luksaddkey", "key", 0, QApplication::UnicodeUTF8));
        radioButtonPassphraseInVolumeFromFile->setText(QApplication::translate("luksaddkey", "key from a keyfile", 0, QApplication::UnicodeUTF8));
        labelExistingPassphrase->setText(QApplication::translate("luksaddkey", "key", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenExistingKeyFile->setToolTip(QApplication::translate("luksaddkey", "open keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenExistingKeyFile->setText(QString());
        pushButtonAdd->setText(QApplication::translate("luksaddkey", "&add", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("luksaddkey", "&cancel", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("luksaddkey", "         key to be added to the encrypted volume", 0, QApplication::UnicodeUTF8));
        radioButtonNewPassphrase->setText(QApplication::translate("luksaddkey", "key", 0, QApplication::UnicodeUTF8));
        radioButtonNewPassphraseFromFile->setText(QApplication::translate("luksaddkey", "key from a keyfile", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonOpenNewKeyFile->setToolTip(QApplication::translate("luksaddkey", "open keyfile", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonOpenNewKeyFile->setText(QString());
        labelNewPassphrase->setText(QApplication::translate("luksaddkey", "key", 0, QApplication::UnicodeUTF8));
        labelReEnterPassphrase->setText(QApplication::translate("luksaddkey", "reenter key", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class luksaddkey: public Ui_luksaddkey {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LUKSADDKEY_H
