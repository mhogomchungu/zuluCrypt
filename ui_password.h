/********************************************************************************
** Form generated from reading UI file 'password.ui'
**
** Created: Mon Sep 12 22:49:17 2011
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
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_PasswordDialog
{
public:
    QPushButton *PushButtonOpen;
    QPushButton *PushButtonCancel;
    QCheckBox *checkBoxReadOnly;
    QPushButton *PushButtonMountPointPath;
    QTextEdit *OpenVolumePath;
    QPushButton *PushButtonVolumePath;
    QTextEdit *MountPointPath;
    QLineEdit *PassPhraseField;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *PasswordDialog)
    {
        if (PasswordDialog->objectName().isEmpty())
            PasswordDialog->setObjectName(QString::fromUtf8("PasswordDialog"));
        PasswordDialog->setWindowModality(Qt::WindowModal);
        PasswordDialog->resize(459, 170);
        PasswordDialog->setFocusPolicy(Qt::TabFocus);
        PasswordDialog->setModal(true);
        PushButtonOpen = new QPushButton(PasswordDialog);
        PushButtonOpen->setObjectName(QString::fromUtf8("PushButtonOpen"));
        PushButtonOpen->setGeometry(QRect(120, 130, 111, 31));
        PushButtonOpen->setDefault(true);
        PushButtonCancel = new QPushButton(PasswordDialog);
        PushButtonCancel->setObjectName(QString::fromUtf8("PushButtonCancel"));
        PushButtonCancel->setGeometry(QRect(240, 130, 101, 31));
        checkBoxReadOnly = new QCheckBox(PasswordDialog);
        checkBoxReadOnly->setObjectName(QString::fromUtf8("checkBoxReadOnly"));
        checkBoxReadOnly->setGeometry(QRect(120, 90, 221, 41));
        checkBoxReadOnly->setChecked(false);
        PushButtonMountPointPath = new QPushButton(PasswordDialog);
        PushButtonMountPointPath->setObjectName(QString::fromUtf8("PushButtonMountPointPath"));
        PushButtonMountPointPath->setGeometry(QRect(420, 40, 31, 31));
        OpenVolumePath = new QTextEdit(PasswordDialog);
        OpenVolumePath->setObjectName(QString::fromUtf8("OpenVolumePath"));
        OpenVolumePath->setGeometry(QRect(100, 10, 321, 31));
        OpenVolumePath->setFocusPolicy(Qt::WheelFocus);
        OpenVolumePath->setLineWidth(1);
        OpenVolumePath->setTabChangesFocus(true);
        OpenVolumePath->setLineWrapMode(QTextEdit::FixedColumnWidth);
        OpenVolumePath->setLineWrapColumnOrWidth(37);
        PushButtonVolumePath = new QPushButton(PasswordDialog);
        PushButtonVolumePath->setObjectName(QString::fromUtf8("PushButtonVolumePath"));
        PushButtonVolumePath->setGeometry(QRect(420, 10, 31, 31));
        MountPointPath = new QTextEdit(PasswordDialog);
        MountPointPath->setObjectName(QString::fromUtf8("MountPointPath"));
        MountPointPath->setGeometry(QRect(100, 40, 321, 31));
        MountPointPath->setFocusPolicy(Qt::WheelFocus);
        MountPointPath->setTabChangesFocus(true);
        MountPointPath->setLineWrapMode(QTextEdit::FixedColumnWidth);
        MountPointPath->setLineWrapColumnOrWidth(37);
        PassPhraseField = new QLineEdit(PasswordDialog);
        PassPhraseField->setObjectName(QString::fromUtf8("PassPhraseField"));
        PassPhraseField->setGeometry(QRect(100, 70, 321, 31));
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
        label_2->setGeometry(QRect(10, 40, 81, 31));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_3 = new QLabel(PasswordDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 70, 81, 31));
        label_3->setFont(font);
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_3->setFrameShape(QFrame::NoFrame);
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        QWidget::setTabOrder(OpenVolumePath, PushButtonVolumePath);
        QWidget::setTabOrder(PushButtonVolumePath, MountPointPath);
        QWidget::setTabOrder(MountPointPath, PushButtonMountPointPath);
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
        checkBoxReadOnly->setText(QApplication::translate("PasswordDialog", "open volume in read only mode", 0, QApplication::UnicodeUTF8));
        PushButtonMountPointPath->setText(QString());
        OpenVolumePath->setHtml(QApplication::translate("PasswordDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:8.5pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:9pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        PushButtonVolumePath->setText(QString());
        label->setText(QApplication::translate("PasswordDialog", "Volume path", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PasswordDialog", "Mount Point", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PasswordDialog", "Passphrase", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PasswordDialog: public Ui_PasswordDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORD_H
