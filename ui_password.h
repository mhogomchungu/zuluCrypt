/********************************************************************************
** Form generated from reading UI file 'password.ui'
**
** Created: Wed Sep 14 18:11:44 2011
**      by: Qt User Interface Compiler version 4.7.4
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
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QGroupBox *groupBox;
    QRadioButton *radioButtonPassPhrase;
    QRadioButton *radioButtonPassPhraseFromFile;
    QPushButton *pushButtonPassPhraseFromFile;
    QLineEdit *OpenVolumePath;
    QLineEdit *MountPointPath;

    void setupUi(QDialog *PasswordDialog)
    {
        if (PasswordDialog->objectName().isEmpty())
            PasswordDialog->setObjectName(QString::fromUtf8("PasswordDialog"));
        PasswordDialog->setWindowModality(Qt::WindowModal);
        PasswordDialog->resize(459, 198);
        PasswordDialog->setFocusPolicy(Qt::TabFocus);
        PasswordDialog->setModal(true);
        PushButtonOpen = new QPushButton(PasswordDialog);
        PushButtonOpen->setObjectName(QString::fromUtf8("PushButtonOpen"));
        PushButtonOpen->setGeometry(QRect(130, 160, 111, 31));
        PushButtonOpen->setDefault(true);
        PushButtonCancel = new QPushButton(PasswordDialog);
        PushButtonCancel->setObjectName(QString::fromUtf8("PushButtonCancel"));
        PushButtonCancel->setGeometry(QRect(240, 160, 101, 31));
        checkBoxReadOnly = new QCheckBox(PasswordDialog);
        checkBoxReadOnly->setObjectName(QString::fromUtf8("checkBoxReadOnly"));
        checkBoxReadOnly->setGeometry(QRect(150, 120, 231, 41));
        checkBoxReadOnly->setChecked(false);
        PushButtonMountPointPath = new QPushButton(PasswordDialog);
        PushButtonMountPointPath->setObjectName(QString::fromUtf8("PushButtonMountPointPath"));
        PushButtonMountPointPath->setGeometry(QRect(420, 40, 31, 31));
        PushButtonVolumePath = new QPushButton(PasswordDialog);
        PushButtonVolumePath->setObjectName(QString::fromUtf8("PushButtonVolumePath"));
        PushButtonVolumePath->setGeometry(QRect(420, 10, 31, 31));
        PassPhraseField = new QLineEdit(PasswordDialog);
        PassPhraseField->setObjectName(QString::fromUtf8("PassPhraseField"));
        PassPhraseField->setGeometry(QRect(100, 100, 321, 31));
        PassPhraseField->setMaxLength(100);
        PassPhraseField->setEchoMode(QLineEdit::Password);
        label = new QLabel(PasswordDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 81, 31));
        QFont font;
        font.setPointSize(9);
        label->setFont(font);
        label->setLayoutDirection(Qt::RightToLeft);
        label->setFrameShadow(QFrame::Raised);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label->setIndent(-1);
        label_2 = new QLabel(PasswordDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 40, 81, 41));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_3 = new QLabel(PasswordDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 100, 81, 31));
        label_3->setFont(font);
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_3->setFrameShape(QFrame::NoFrame);
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        groupBox = new QGroupBox(PasswordDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(100, 70, 321, 31));
        radioButtonPassPhrase = new QRadioButton(groupBox);
        radioButtonPassPhrase->setObjectName(QString::fromUtf8("radioButtonPassPhrase"));
        radioButtonPassPhrase->setGeometry(QRect(20, 10, 95, 21));
        radioButtonPassPhrase->setChecked(true);
        radioButtonPassPhraseFromFile = new QRadioButton(groupBox);
        radioButtonPassPhraseFromFile->setObjectName(QString::fromUtf8("radioButtonPassPhraseFromFile"));
        radioButtonPassPhraseFromFile->setGeometry(QRect(130, 10, 151, 21));
        radioButtonPassPhraseFromFile->setChecked(false);
        pushButtonPassPhraseFromFile = new QPushButton(PasswordDialog);
        pushButtonPassPhraseFromFile->setObjectName(QString::fromUtf8("pushButtonPassPhraseFromFile"));
        pushButtonPassPhraseFromFile->setGeometry(QRect(420, 100, 31, 31));
        OpenVolumePath = new QLineEdit(PasswordDialog);
        OpenVolumePath->setObjectName(QString::fromUtf8("OpenVolumePath"));
        OpenVolumePath->setGeometry(QRect(100, 10, 321, 31));
        MountPointPath = new QLineEdit(PasswordDialog);
        MountPointPath->setObjectName(QString::fromUtf8("MountPointPath"));
        MountPointPath->setGeometry(QRect(100, 40, 321, 31));
        QWidget::setTabOrder(PushButtonVolumePath, PushButtonMountPointPath);
        QWidget::setTabOrder(PushButtonMountPointPath, PassPhraseField);
        QWidget::setTabOrder(PassPhraseField, checkBoxReadOnly);
        QWidget::setTabOrder(checkBoxReadOnly, PushButtonOpen);
        QWidget::setTabOrder(PushButtonOpen, PushButtonCancel);

        retranslateUi(PasswordDialog);

        QMetaObject::connectSlotsByName(PasswordDialog);
    } // setupUi

    void retranslateUi(QDialog *PasswordDialog)
    {
        PasswordDialog->setWindowTitle(QApplication::translate("PasswordDialog", "Enter Passphrase", 0, QApplication::UnicodeUTF8));
        PushButtonOpen->setText(QApplication::translate("PasswordDialog", "open", 0, QApplication::UnicodeUTF8));
        PushButtonCancel->setText(QApplication::translate("PasswordDialog", "cancel", 0, QApplication::UnicodeUTF8));
        checkBoxReadOnly->setText(QApplication::translate("PasswordDialog", "open in read only mode", 0, QApplication::UnicodeUTF8));
        PushButtonMountPointPath->setText(QString());
        PushButtonVolumePath->setText(QString());
        label->setText(QApplication::translate("PasswordDialog", "Volume path", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PasswordDialog", "Mount Point", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PasswordDialog", "Passphrase", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        radioButtonPassPhrase->setText(QApplication::translate("PasswordDialog", "passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonPassPhraseFromFile->setText(QApplication::translate("PasswordDialog", "passphrase from file", 0, QApplication::UnicodeUTF8));
        pushButtonPassPhraseFromFile->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PasswordDialog: public Ui_PasswordDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORD_H
