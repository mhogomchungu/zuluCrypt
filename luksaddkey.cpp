#include "ui_luksaddkey.h"
#include "luksaddkey.h"

#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

luksaddkeyUI::luksaddkeyUI(QWidget *parent) :   QDialog(parent)
{
	ui.setupUi(this);

	this->setFixedSize(this->size());

	connect(ui.pushButtonOpenFile,SIGNAL(clicked()), this,SLOT(pbOpenFile())) ;

	connect(ui.pushButtonOpenExistingKeyFile,SIGNAL(clicked()),this, SLOT(pbOpenExisitingKeyFile())) ;

	connect(ui.pushButtonOpenNewKeyFile,(SIGNAL(clicked())),this, SLOT(pbOpenNewKeyFile())) ;

	connect(ui.pushButtonOpenPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition(void))) ;

	connect(ui.pushButtonAdd,SIGNAL(clicked()), this, SLOT(pbAdd())) ;

	connect(ui.pushButtonCancel,SIGNAL(clicked()),SLOT(pbCancel())) ;

	connect(ui.radioButtonNewPassphrase,SIGNAL(toggled(bool)),SLOT(rbNewPassphrase())) ;

	connect(ui.radioButtonNewPassphraseFromFile,SIGNAL(toggled(bool)),SLOT(rbNewPassphraseFromFile())) ;

	connect(ui.radioButtonPassphraseinVolume,SIGNAL(toggled(bool)),SLOT(rbExistingPassphrase())) ;

	connect(ui.radioButtonPassphraseInVolumeFromFile,SIGNAL(toggled(bool)),SLOT(rbExistingPassphraseFromFile())) ;

	ui.textEditExistingPassphrase->clear();
	ui.textEditPassphraseToAdd->clear();
	ui.textEditPathToVolume->clear();
	ui.radioButtonNewPassphrase->setChecked(true);
	ui.radioButtonPassphraseinVolume->setChecked(true);
}

void luksaddkeyUI::partitionEntry(QString partition)
{
	ui.textEditPathToVolume->setText(partition);
	ShowUI() ;
}

void luksaddkeyUI::HideUI()
{
	this->hide();
	ui.textEditExistingPassphrase->clear();
	ui.textEditPassphraseToAdd->clear();
	ui.textEditPathToVolume->clear();
	ui.radioButtonNewPassphrase->setChecked(true);
	ui.radioButtonPassphraseinVolume->setChecked(true);
}

void luksaddkeyUI::ShowUI()
{
	this->show();
}

void luksaddkeyUI::pbOpenExisitingKeyFile(void)
{	
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("existing key file"),QDir::homePath(),0);
	ui.textEditExistingPassphrase->setText( Z ) ;
}

void luksaddkeyUI::pbOpenNewKeyFile(void)
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("new key file"),QDir::homePath(),0);
	ui.textEditPassphraseToAdd->setText( Z ) ;
}

void luksaddkeyUI::pbOpenFile(void)
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("encrypted volume path"),QDir::homePath(),0);
	ui.textEditPathToVolume->setText( Z ) ;
}

void luksaddkeyUI::pbOpenPartition(void)
{
	emit pbOpenPartitionClicked() ;
}

void luksaddkeyUI::rbExistingPassphrase(void)
{
	ui.textEditExistingPassphrase->setEchoMode(QLineEdit::Password);
	ui.pushButtonOpenExistingKeyFile->setEnabled(false);
	ui.labelExistingPassphrase->setText(QString("passphrase")) ;
	ui.textEditExistingPassphrase->clear();
}

void luksaddkeyUI::rbExistingPassphraseFromFile(void)
{
	ui.textEditExistingPassphrase->setEchoMode(QLineEdit::Normal);
	ui.pushButtonOpenExistingKeyFile->setEnabled(true);
	ui.labelExistingPassphrase->setText(QString("key file")) ;
	ui.textEditExistingPassphrase->clear();
}

void luksaddkeyUI::rbNewPassphrase(void)
{
	ui.textEditPassphraseToAdd->setEchoMode(QLineEdit::Password);
	ui.pushButtonOpenNewKeyFile->setEnabled(false);
	ui.labelNewPassphrase->setText(QString("passphrase")) ;
	ui.textEditPassphraseToAdd->clear();
}

void luksaddkeyUI::rbNewPassphraseFromFile()
{
	ui.textEditPassphraseToAdd->setEchoMode(QLineEdit::Normal);
	ui.pushButtonOpenNewKeyFile->setEnabled(true);
	ui.labelNewPassphrase->setText(QString("key file")) ;
}

void luksaddkeyUI::pbAdd(void)
{
	QString a = ui.textEditPathToVolume->text() ;
	QString b = ui.textEditExistingPassphrase->text() ;
	QString c = ui.textEditPassphraseToAdd->text() ;

	bool x = ui.radioButtonPassphraseInVolumeFromFile->isChecked() ;
	bool y = ui.radioButtonNewPassphraseFromFile->isChecked() ;

	QMessageBox m ;

	if ( a.isEmpty() == true ){
		m.setText(QString("ERROR: the encrypted volume path field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	if ( b.isEmpty() == true ){
		m.setText(QString("ERROR: existing passphrase field is empth"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}
	if ( c.isEmpty() == true ){
		m.setText(QString("ERROR: new passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		return ;
	}

	HideUI() ;
	emit clickedpbAdd(a , x , b , y , c  ) ;
}

void luksaddkeyUI::pbCancel(void)
{
	HideUI() ;
}

luksaddkeyUI::~luksaddkeyUI()
{

}
