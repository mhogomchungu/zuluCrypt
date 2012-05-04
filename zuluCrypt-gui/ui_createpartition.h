/********************************************************************************
** Form generated from reading UI file 'createpartition.ui'
**
** Created: Fri May 4 19:14:32 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEPARTITION_H
#define UI_CREATEPARTITION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_createpartition
{
public:
    QLineEdit *lineEditVolumePath;
    QLabel *labelVolumePath;
    QPushButton *pbCreate;
    QPushButton *pbCancel;
    QLineEdit *lineEditPassphrase1;
    QLabel *labelPassPhrase;
    QLineEdit *lineEditPassPhrase2;
    QLabel *labelRepeatPassPhrase;
    QPushButton *pbOpenKeyFile;
    QGroupBox *groupBox_3;
    QRadioButton *rbPassphrase;
    QRadioButton *rbPassphraseFromFile;
    QLabel *labelfs;
    QComboBox *comboBoxFS;
    QLabel *labelvolumetype;
    QComboBox *comboBoxVolumeType;
    QLabel *labelrng;
    QComboBox *comboBoxRNG;

    void setupUi(QWidget *createpartition)
    {
        if (createpartition->objectName().isEmpty())
            createpartition->setObjectName(QString::fromUtf8("createpartition"));
        createpartition->setWindowModality(Qt::ApplicationModal);
        createpartition->resize(560, 242);
        lineEditVolumePath = new QLineEdit(createpartition);
        lineEditVolumePath->setObjectName(QString::fromUtf8("lineEditVolumePath"));
        lineEditVolumePath->setGeometry(QRect(130, 10, 301, 31));
        labelVolumePath = new QLabel(createpartition);
        labelVolumePath->setObjectName(QString::fromUtf8("labelVolumePath"));
        labelVolumePath->setGeometry(QRect(0, 10, 121, 31));
        labelVolumePath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pbCreate = new QPushButton(createpartition);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(190, 200, 91, 31));
        pbCreate->setAutoDefault(false);
        pbCreate->setDefault(false);
        pbCancel = new QPushButton(createpartition);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(280, 200, 91, 31));
        pbCancel->setDefault(false);
        lineEditPassphrase1 = new QLineEdit(createpartition);
        lineEditPassphrase1->setObjectName(QString::fromUtf8("lineEditPassphrase1"));
        lineEditPassphrase1->setGeometry(QRect(130, 80, 301, 31));
        labelPassPhrase = new QLabel(createpartition);
        labelPassPhrase->setObjectName(QString::fromUtf8("labelPassPhrase"));
        labelPassPhrase->setGeometry(QRect(0, 80, 121, 31));
        labelPassPhrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditPassPhrase2 = new QLineEdit(createpartition);
        lineEditPassPhrase2->setObjectName(QString::fromUtf8("lineEditPassPhrase2"));
        lineEditPassPhrase2->setGeometry(QRect(130, 110, 301, 31));
        labelRepeatPassPhrase = new QLabel(createpartition);
        labelRepeatPassPhrase->setObjectName(QString::fromUtf8("labelRepeatPassPhrase"));
        labelRepeatPassPhrase->setGeometry(QRect(0, 110, 121, 31));
        labelRepeatPassPhrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pbOpenKeyFile = new QPushButton(createpartition);
        pbOpenKeyFile->setObjectName(QString::fromUtf8("pbOpenKeyFile"));
        pbOpenKeyFile->setGeometry(QRect(430, 80, 31, 31));
        groupBox_3 = new QGroupBox(createpartition);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 40, 521, 41));
        rbPassphrase = new QRadioButton(groupBox_3);
        rbPassphrase->setObjectName(QString::fromUtf8("rbPassphrase"));
        rbPassphrase->setGeometry(QRect(110, 10, 95, 21));
        rbPassphraseFromFile = new QRadioButton(groupBox_3);
        rbPassphraseFromFile->setObjectName(QString::fromUtf8("rbPassphraseFromFile"));
        rbPassphraseFromFile->setGeometry(QRect(220, 10, 201, 21));
        labelfs = new QLabel(createpartition);
        labelfs->setObjectName(QString::fromUtf8("labelfs"));
        labelfs->setGeometry(QRect(110, 140, 111, 31));
        labelfs->setAlignment(Qt::AlignCenter);
        comboBoxFS = new QComboBox(createpartition);
        comboBoxFS->setObjectName(QString::fromUtf8("comboBoxFS"));
        comboBoxFS->setGeometry(QRect(110, 170, 111, 31));
        labelvolumetype = new QLabel(createpartition);
        labelvolumetype->setObjectName(QString::fromUtf8("labelvolumetype"));
        labelvolumetype->setGeometry(QRect(220, 140, 111, 31));
        labelvolumetype->setAlignment(Qt::AlignCenter);
        comboBoxVolumeType = new QComboBox(createpartition);
        comboBoxVolumeType->insertItems(0, QStringList()
         << QString::fromUtf8("luks")
         << QString::fromUtf8("plain")
        );
        comboBoxVolumeType->setObjectName(QString::fromUtf8("comboBoxVolumeType"));
        comboBoxVolumeType->setGeometry(QRect(220, 170, 121, 31));
        comboBoxVolumeType->setLayoutDirection(Qt::LeftToRight);
        labelrng = new QLabel(createpartition);
        labelrng->setObjectName(QString::fromUtf8("labelrng"));
        labelrng->setGeometry(QRect(330, 140, 121, 31));
        labelrng->setAlignment(Qt::AlignCenter);
        comboBoxRNG = new QComboBox(createpartition);
        comboBoxRNG->insertItems(0, QStringList()
         << QString::fromUtf8("/dev/urandom")
         << QString::fromUtf8("/dev/random")
        );
        comboBoxRNG->setObjectName(QString::fromUtf8("comboBoxRNG"));
        comboBoxRNG->setGeometry(QRect(340, 170, 111, 31));
        QWidget::setTabOrder(lineEditVolumePath, rbPassphrase);
        QWidget::setTabOrder(rbPassphrase, rbPassphraseFromFile);
        QWidget::setTabOrder(rbPassphraseFromFile, lineEditPassphrase1);
        QWidget::setTabOrder(lineEditPassphrase1, pbOpenKeyFile);
        QWidget::setTabOrder(pbOpenKeyFile, lineEditPassPhrase2);
        QWidget::setTabOrder(lineEditPassPhrase2, comboBoxFS);
        QWidget::setTabOrder(comboBoxFS, comboBoxVolumeType);
        QWidget::setTabOrder(comboBoxVolumeType, comboBoxRNG);
        QWidget::setTabOrder(comboBoxRNG, pbCreate);
        QWidget::setTabOrder(pbCreate, pbCancel);

        retranslateUi(createpartition);

        QMetaObject::connectSlotsByName(createpartition);
    } // setupUi

    void retranslateUi(QWidget *createpartition)
    {
        createpartition->setWindowTitle(QApplication::translate("createpartition", "create a new volume", 0, QApplication::UnicodeUTF8));
        labelVolumePath->setText(QApplication::translate("createpartition", "path to volume", 0, QApplication::UnicodeUTF8));
        pbCreate->setText(QApplication::translate("createpartition", "c&reate", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("createpartition", "&cancel", 0, QApplication::UnicodeUTF8));
        labelPassPhrase->setText(QApplication::translate("createpartition", "passphrase", 0, QApplication::UnicodeUTF8));
        labelRepeatPassPhrase->setText(QApplication::translate("createpartition", "repeat passphrase", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pbOpenKeyFile->setToolTip(QApplication::translate("createpartition", "open a key file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbOpenKeyFile->setText(QString());
        groupBox_3->setTitle(QString());
        rbPassphrase->setText(QApplication::translate("createpartition", "passphrase", 0, QApplication::UnicodeUTF8));
        rbPassphraseFromFile->setText(QApplication::translate("createpartition", "passphrase from a key file", 0, QApplication::UnicodeUTF8));
        labelfs->setText(QApplication::translate("createpartition", "file system type", 0, QApplication::UnicodeUTF8));
        labelvolumetype->setText(QApplication::translate("createpartition", "volume type", 0, QApplication::UnicodeUTF8));
        labelrng->setText(QApplication::translate("createpartition", "rng", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class createpartition: public Ui_createpartition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEPARTITION_H
