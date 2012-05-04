/********************************************************************************
** Form generated from reading UI file 'password.ui'
**
** Created: Fri May 4 07:36:14 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PASSWORD_H
#define UI_PASSWORD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_PasswordDialog
{
public:
    QPushButton *PushButtonOpen;
    QPushButton *PushButtonCancel;
    QCheckBox *checkBoxReadOnly;
    QPushButton *PushButtonMountPointPath;
    QPushButton *PushButtonVolumePath;
    QLineEdit *PassPhraseField;
    QGroupBox *groupBox;
    QRadioButton *radioButtonPassPhrase;
    QRadioButton *radioButtonPassPhraseFromFile;
    QPushButton *pushButtonPassPhraseFromFile;
    QLineEdit *OpenVolumePath;
    QLineEdit *MountPointPath;
    QLabel *labelPassphrase;
    QLabel *labelMoutPointPath;
    QLabel *labelVolumePath;

    void setupUi(QDialog *PasswordDialog)
    {
        if (PasswordDialog->objectName().isEmpty())
            PasswordDialog->setObjectName(QString::fromUtf8("PasswordDialog"));
        PasswordDialog->setWindowModality(Qt::ApplicationModal);
        PasswordDialog->resize(511, 198);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        PasswordDialog->setFont(font);
        PasswordDialog->setFocusPolicy(Qt::TabFocus);
        PasswordDialog->setModal(true);
        PushButtonOpen = new QPushButton(PasswordDialog);
        PushButtonOpen->setObjectName(QString::fromUtf8("PushButtonOpen"));
        PushButtonOpen->setGeometry(QRect(160, 160, 101, 31));
        PushButtonOpen->setAutoDefault(false);
        PushButtonOpen->setDefault(true);
        PushButtonCancel = new QPushButton(PasswordDialog);
        PushButtonCancel->setObjectName(QString::fromUtf8("PushButtonCancel"));
        PushButtonCancel->setGeometry(QRect(260, 160, 91, 31));
        PushButtonCancel->setAutoDefault(false);
        PushButtonCancel->setDefault(false);
        checkBoxReadOnly = new QCheckBox(PasswordDialog);
        checkBoxReadOnly->setObjectName(QString::fromUtf8("checkBoxReadOnly"));
        checkBoxReadOnly->setGeometry(QRect(170, 130, 181, 31));
        checkBoxReadOnly->setChecked(false);
        PushButtonMountPointPath = new QPushButton(PasswordDialog);
        PushButtonMountPointPath->setObjectName(QString::fromUtf8("PushButtonMountPointPath"));
        PushButtonMountPointPath->setGeometry(QRect(410, 40, 41, 31));
        PushButtonMountPointPath->setAutoDefault(false);
        PushButtonVolumePath = new QPushButton(PasswordDialog);
        PushButtonVolumePath->setObjectName(QString::fromUtf8("PushButtonVolumePath"));
        PushButtonVolumePath->setGeometry(QRect(410, 10, 41, 31));
        PushButtonVolumePath->setAutoDefault(false);
        PushButtonVolumePath->setFlat(false);
        PassPhraseField = new QLineEdit(PasswordDialog);
        PassPhraseField->setObjectName(QString::fromUtf8("PassPhraseField"));
        PassPhraseField->setGeometry(QRect(100, 100, 311, 31));
        PassPhraseField->setMaxLength(100);
        PassPhraseField->setEchoMode(QLineEdit::Password);
        groupBox = new QGroupBox(PasswordDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(100, 70, 311, 31));
        radioButtonPassPhrase = new QRadioButton(groupBox);
        radioButtonPassPhrase->setObjectName(QString::fromUtf8("radioButtonPassPhrase"));
        radioButtonPassPhrase->setGeometry(QRect(10, 0, 95, 31));
        radioButtonPassPhrase->setChecked(true);
        radioButtonPassPhraseFromFile = new QRadioButton(groupBox);
        radioButtonPassPhraseFromFile->setObjectName(QString::fromUtf8("radioButtonPassPhraseFromFile"));
        radioButtonPassPhraseFromFile->setGeometry(QRect(110, 0, 201, 31));
        radioButtonPassPhraseFromFile->setChecked(false);
        pushButtonPassPhraseFromFile = new QPushButton(PasswordDialog);
        pushButtonPassPhraseFromFile->setObjectName(QString::fromUtf8("pushButtonPassPhraseFromFile"));
        pushButtonPassPhraseFromFile->setGeometry(QRect(410, 100, 41, 31));
        pushButtonPassPhraseFromFile->setAutoDefault(false);
        OpenVolumePath = new QLineEdit(PasswordDialog);
        OpenVolumePath->setObjectName(QString::fromUtf8("OpenVolumePath"));
        OpenVolumePath->setGeometry(QRect(100, 10, 311, 31));
        MountPointPath = new QLineEdit(PasswordDialog);
        MountPointPath->setObjectName(QString::fromUtf8("MountPointPath"));
        MountPointPath->setGeometry(QRect(100, 40, 311, 31));
        labelPassphrase = new QLabel(PasswordDialog);
        labelPassphrase->setObjectName(QString::fromUtf8("labelPassphrase"));
        labelPassphrase->setGeometry(QRect(0, 100, 91, 31));
        labelPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelMoutPointPath = new QLabel(PasswordDialog);
        labelMoutPointPath->setObjectName(QString::fromUtf8("labelMoutPointPath"));
        labelMoutPointPath->setGeometry(QRect(0, 39, 91, 31));
        labelMoutPointPath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelVolumePath = new QLabel(PasswordDialog);
        labelVolumePath->setObjectName(QString::fromUtf8("labelVolumePath"));
        labelVolumePath->setGeometry(QRect(0, 10, 91, 31));
        labelVolumePath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        QWidget::setTabOrder(OpenVolumePath, PushButtonVolumePath);
        QWidget::setTabOrder(PushButtonVolumePath, MountPointPath);
        QWidget::setTabOrder(MountPointPath, PushButtonMountPointPath);
        QWidget::setTabOrder(PushButtonMountPointPath, radioButtonPassPhrase);
        QWidget::setTabOrder(radioButtonPassPhrase, radioButtonPassPhraseFromFile);
        QWidget::setTabOrder(radioButtonPassPhraseFromFile, PassPhraseField);
        QWidget::setTabOrder(PassPhraseField, pushButtonPassPhraseFromFile);
        QWidget::setTabOrder(pushButtonPassPhraseFromFile, checkBoxReadOnly);
        QWidget::setTabOrder(checkBoxReadOnly, PushButtonOpen);
        QWidget::setTabOrder(PushButtonOpen, PushButtonCancel);

        retranslateUi(PasswordDialog);

        QMetaObject::connectSlotsByName(PasswordDialog);
    } // setupUi

    void retranslateUi(QDialog *PasswordDialog)
    {
        PasswordDialog->setWindowTitle(QApplication::translate("PasswordDialog", "open encrypted volume", 0, QApplication::UnicodeUTF8));
        PushButtonOpen->setText(QApplication::translate("PasswordDialog", "&open", 0, QApplication::UnicodeUTF8));
        PushButtonCancel->setText(QApplication::translate("PasswordDialog", "&cancel", 0, QApplication::UnicodeUTF8));
        checkBoxReadOnly->setText(QApplication::translate("PasswordDialog", "open in read only mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButtonMountPointPath->setToolTip(QApplication::translate("PasswordDialog", "select mount point path", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButtonMountPointPath->setText(QString());
#ifndef QT_NO_TOOLTIP
        PushButtonVolumePath->setToolTip(QApplication::translate("PasswordDialog", "open volume path", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButtonVolumePath->setText(QString());
        groupBox->setTitle(QString());
        radioButtonPassPhrase->setText(QApplication::translate("PasswordDialog", "passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonPassPhraseFromFile->setText(QApplication::translate("PasswordDialog", "passphrase from a key file", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonPassPhraseFromFile->setToolTip(QApplication::translate("PasswordDialog", "open key file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonPassPhraseFromFile->setText(QString());
        labelPassphrase->setText(QApplication::translate("PasswordDialog", "passphrase", 0, QApplication::UnicodeUTF8));
        labelMoutPointPath->setText(QApplication::translate("PasswordDialog", "mount point", 0, QApplication::UnicodeUTF8));
        labelVolumePath->setText(QApplication::translate("PasswordDialog", "volume path", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PasswordDialog: public Ui_PasswordDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORD_H
