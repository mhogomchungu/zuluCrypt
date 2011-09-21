#include "luksdeletekey.h"

#include <QObject>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

luksdeletekey::luksdeletekey(QWidget *parent) :  QDialog(parent)
{
	ui.setupUi(this);
	this->setFixedSize(this->size());
	connect(ui.pushButtonDelete,SIGNAL(clicked()),this,SLOT(pbDelete())) ;
	connect(ui.pushButtonCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(ui.rbPassphrase,SIGNAL(toggled(bool)),this,SLOT(rbPassphrase())) ;
	connect(ui.rbPassphraseFromFile,SIGNAL(toggled(bool)),this, SLOT(rbPassphraseFromFile())) ;
	connect(ui.pushButtonOpenKeyFile,SIGNAL(clicked()),this,SLOT(pbOpenKeyFile())) ;
	connect(ui.pushButtonOpenVolume,SIGNAL(clicked()),this,SLOT(pbOpenVolume()));
}

void luksdeletekey::rbPassphrase()
{
	ui.labelPassphrase->setText(QString("passphrase"));
	ui.lineEditPassphrase->setEchoMode(QLineEdit::Password);
	ui.lineEditPassphrase->clear();
	ui.pushButtonOpenKeyFile->setEnabled(false);
}

void luksdeletekey::rbPassphraseFromFile()
{
	ui.labelPassphrase->setText(QString("keyfile"));
	ui.lineEditPassphrase->setEchoMode(QLineEdit::Normal);
	ui.lineEditPassphrase->clear();
	ui.pushButtonOpenKeyFile->setEnabled(true);
}

void luksdeletekey::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("key file with a passphrase to delete"),QDir::homePath(),0);
	ui.lineEditPassphrase->setText( Z );
}

void luksdeletekey::ShowUI()
{
	ui.rbPassphrase->setEnabled(true);
	ui.labelPassphrase->setText(QString("passphrase"));
	ui.rbPassphrase->setChecked(true);
	ui.lineEditVolumePath->setFocus();
	ui.pushButtonCancel->setDefault(true);
	this->show();
}

void luksdeletekey::pbCancel()
{
	HideUI() ;
}

void luksdeletekey::pbDelete()
{
	QMessageBox m ;
	if ( ui.lineEditPassphrase->text().isEmpty() == true ){
		m.setText(QString("ERROR: the passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	QString Z = ui.lineEditPassphrase->text() ;
	QString N = ui.lineEditVolumePath->text() ;
	bool P = ui.rbPassphraseFromFile->isChecked() ;
	HideUI() ;
	emit pbDeleteClicked( N, P, Z );
}

void luksdeletekey::pbOpenVolume()
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("volume path"),QDir::homePath(),0);
	ui.lineEditVolumePath->setText( Z );
}

void luksdeletekey::deleteKey(QString passphrase)
{
	ui.lineEditVolumePath->setText(passphrase);
	ShowUI() ;
}

void luksdeletekey::HideUI()
{
	ui.lineEditPassphrase->clear();
	ui.lineEditVolumePath->clear();
	this->hide();
}

luksdeletekey::~luksdeletekey()
{
}
