/********************************************************************************
** Form generated from reading UI file 'createpartition.ui'
**
** Created: Fri Oct 14 23:07:27 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEPARTITION_H
#define UI_CREATEPARTITION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
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
    QGroupBox *groupBox;
    QRadioButton *rbPlain;
    QRadioButton *rbLuks;
    QGroupBox *groupBox_2;
    QRadioButton *rbext3;
    QRadioButton *rbext4;
    QRadioButton *rbfat;
    QPushButton *pbCreate;
    QPushButton *pbCancel;
    QLineEdit *lineEditPassphrase1;
    QLabel *labelPassPhrase;
    QLineEdit *lineEditPassPhrase2;
    QLabel *label_3;
    QPushButton *pbOpenKeyFile;
    QGroupBox *groupBox_3;
    QRadioButton *rbPassphrase;
    QRadioButton *rbPassphraseFromFile;

    void setupUi(QWidget *createpartition)
    {
        if (createpartition->objectName().isEmpty())
            createpartition->setObjectName(QString::fromUtf8("createpartition"));
        createpartition->resize(441, 227);
        lineEditVolumePath = new QLineEdit(createpartition);
        lineEditVolumePath->setObjectName(QString::fromUtf8("lineEditVolumePath"));
        lineEditVolumePath->setGeometry(QRect(120, 10, 311, 31));
        labelVolumePath = new QLabel(createpartition);
        labelVolumePath->setObjectName(QString::fromUtf8("labelVolumePath"));
        labelVolumePath->setGeometry(QRect(10, 10, 111, 31));
        labelVolumePath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        groupBox = new QGroupBox(createpartition);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 140, 211, 61));
        rbPlain = new QRadioButton(groupBox);
        rbPlain->setObjectName(QString::fromUtf8("rbPlain"));
        rbPlain->setGeometry(QRect(40, 30, 61, 21));
        rbLuks = new QRadioButton(groupBox);
        rbLuks->setObjectName(QString::fromUtf8("rbLuks"));
        rbLuks->setGeometry(QRect(100, 30, 71, 21));
        groupBox_2 = new QGroupBox(createpartition);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(220, 140, 211, 51));
        rbext3 = new QRadioButton(groupBox_2);
        rbext3->setObjectName(QString::fromUtf8("rbext3"));
        rbext3->setGeometry(QRect(10, 30, 95, 21));
        rbext4 = new QRadioButton(groupBox_2);
        rbext4->setObjectName(QString::fromUtf8("rbext4"));
        rbext4->setGeometry(QRect(70, 30, 61, 21));
        rbfat = new QRadioButton(groupBox_2);
        rbfat->setObjectName(QString::fromUtf8("rbfat"));
        rbfat->setGeometry(QRect(130, 30, 61, 21));
        pbCreate = new QPushButton(createpartition);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        pbCreate->setGeometry(QRect(130, 190, 91, 31));
        pbCancel = new QPushButton(createpartition);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(220, 190, 91, 31));
        pbCancel->setDefault(true);
        lineEditPassphrase1 = new QLineEdit(createpartition);
        lineEditPassphrase1->setObjectName(QString::fromUtf8("lineEditPassphrase1"));
        lineEditPassphrase1->setGeometry(QRect(120, 80, 281, 31));
        labelPassPhrase = new QLabel(createpartition);
        labelPassPhrase->setObjectName(QString::fromUtf8("labelPassPhrase"));
        labelPassPhrase->setGeometry(QRect(20, 80, 101, 31));
        labelPassPhrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditPassPhrase2 = new QLineEdit(createpartition);
        lineEditPassPhrase2->setObjectName(QString::fromUtf8("lineEditPassPhrase2"));
        lineEditPassPhrase2->setGeometry(QRect(120, 110, 281, 31));
        label_3 = new QLabel(createpartition);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 110, 111, 31));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pbOpenKeyFile = new QPushButton(createpartition);
        pbOpenKeyFile->setObjectName(QString::fromUtf8("pbOpenKeyFile"));
        pbOpenKeyFile->setGeometry(QRect(400, 80, 31, 31));
        groupBox_3 = new QGroupBox(createpartition);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 40, 421, 41));
        rbPassphrase = new QRadioButton(groupBox_3);
        rbPassphrase->setObjectName(QString::fromUtf8("rbPassphrase"));
        rbPassphrase->setGeometry(QRect(70, 10, 95, 21));
        rbPassphraseFromFile = new QRadioButton(groupBox_3);
        rbPassphraseFromFile->setObjectName(QString::fromUtf8("rbPassphraseFromFile"));
        rbPassphraseFromFile->setGeometry(QRect(170, 10, 211, 21));

        retranslateUi(createpartition);

        QMetaObject::connectSlotsByName(createpartition);
    } // setupUi

    void retranslateUi(QWidget *createpartition)
    {
        createpartition->setWindowTitle(QApplication::translate("createpartition", "create a new volume", 0, QApplication::UnicodeUTF8));
        labelVolumePath->setText(QApplication::translate("createpartition", "path to volume", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("createpartition", "volume type", 0, QApplication::UnicodeUTF8));
        rbPlain->setText(QApplication::translate("createpartition", "plain", 0, QApplication::UnicodeUTF8));
        rbLuks->setText(QApplication::translate("createpartition", "luks", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("createpartition", "file system type", 0, QApplication::UnicodeUTF8));
        rbext3->setText(QApplication::translate("createpartition", "ext3", 0, QApplication::UnicodeUTF8));
        rbext4->setText(QApplication::translate("createpartition", "ext4", 0, QApplication::UnicodeUTF8));
        rbfat->setText(QApplication::translate("createpartition", "fat", 0, QApplication::UnicodeUTF8));
        pbCreate->setText(QApplication::translate("createpartition", "create", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("createpartition", "cancel", 0, QApplication::UnicodeUTF8));
        labelPassPhrase->setText(QApplication::translate("createpartition", "passphrase", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("createpartition", "repeat passphrase", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pbOpenKeyFile->setToolTip(QApplication::translate("createpartition", "open a key file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbOpenKeyFile->setText(QString());
        groupBox_3->setTitle(QString());
        rbPassphrase->setText(QApplication::translate("createpartition", "passphrase", 0, QApplication::UnicodeUTF8));
        rbPassphraseFromFile->setText(QApplication::translate("createpartition", "passphrase from a key file", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class createpartition: public Ui_createpartition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEPARTITION_H
