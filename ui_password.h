/********************************************************************************
** Form generated from reading UI file 'password.ui'
**
** Created: Tue Sep 20 20:54:44 2011
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
    QLabel *labelVolumePath;
    QLabel *labelMoutPointPath;
    QLabel *labelPassphrase;
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
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        PasswordDialog->setFont(font);
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
        PushButtonVolumePath->setFlat(false);
        PassPhraseField = new QLineEdit(PasswordDialog);
        PassPhraseField->setObjectName(QString::fromUtf8("PassPhraseField"));
        PassPhraseField->setGeometry(QRect(100, 100, 321, 31));
        PassPhraseField->setMaxLength(100);
        PassPhraseField->setEchoMode(QLineEdit::Password);
        labelVolumePath = new QLabel(PasswordDialog);
        labelVolumePath->setObjectName(QString::fromUtf8("labelVolumePath"));
        labelVolumePath->setGeometry(QRect(10, 10, 81, 31));
        QFont font1;
        font1.setPointSize(9);
        labelVolumePath->setFont(font1);
        labelVolumePath->setLayoutDirection(Qt::RightToLeft);
        labelVolumePath->setFrameShadow(QFrame::Raised);
        labelVolumePath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelVolumePath->setIndent(-1);
        labelMoutPointPath = new QLabel(PasswordDialog);
        labelMoutPointPath->setObjectName(QString::fromUtf8("labelMoutPointPath"));
        labelMoutPointPath->setGeometry(QRect(10, 40, 81, 31));
        QFont font2;
        font2.setPointSize(9);
        font2.setBold(false);
        font2.setWeight(50);
        labelMoutPointPath->setFont(font2);
        labelMoutPointPath->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        labelPassphrase = new QLabel(PasswordDialog);
        labelPassphrase->setObjectName(QString::fromUtf8("labelPassphrase"));
        labelPassphrase->setGeometry(QRect(10, 100, 81, 31));
        labelPassphrase->setFont(font1);
        labelPassphrase->setLayoutDirection(Qt::LeftToRight);
        labelPassphrase->setFrameShape(QFrame::NoFrame);
        labelPassphrase->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        groupBox = new QGroupBox(PasswordDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(100, 70, 321, 31));
        radioButtonPassPhrase = new QRadioButton(groupBox);
        radioButtonPassPhrase->setObjectName(QString::fromUtf8("radioButtonPassPhrase"));
        radioButtonPassPhrase->setGeometry(QRect(20, 10, 95, 21));
        radioButtonPassPhrase->setChecked(true);
        radioButtonPassPhraseFromFile = new QRadioButton(groupBox);
        radioButtonPassPhraseFromFile->setObjectName(QString::fromUtf8("radioButtonPassPhraseFromFile"));
        radioButtonPassPhraseFromFile->setGeometry(QRect(130, 10, 181, 21));
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
#ifndef QT_NO_TOOLTIP
        PushButtonMountPointPath->setToolTip(QApplication::translate("PasswordDialog", "select mount point path", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButtonMountPointPath->setText(QString());
#ifndef QT_NO_TOOLTIP
        PushButtonVolumePath->setToolTip(QApplication::translate("PasswordDialog", "open volume path", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButtonVolumePath->setText(QString());
        labelVolumePath->setText(QApplication::translate("PasswordDialog", "Volume path", 0, QApplication::UnicodeUTF8));
        labelMoutPointPath->setText(QApplication::translate("PasswordDialog", "Mount Point", 0, QApplication::UnicodeUTF8));
        labelPassphrase->setText(QApplication::translate("PasswordDialog", "Passphrase", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        radioButtonPassPhrase->setText(QApplication::translate("PasswordDialog", "passphrase", 0, QApplication::UnicodeUTF8));
        radioButtonPassPhraseFromFile->setText(QApplication::translate("PasswordDialog", "passphrase from key file", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButtonPassPhraseFromFile->setToolTip(QApplication::translate("PasswordDialog", "open key file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButtonPassPhraseFromFile->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PasswordDialog: public Ui_PasswordDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORD_H
