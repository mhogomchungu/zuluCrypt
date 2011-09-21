/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


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

	ui.lineEditReEnterPassphrase->setEchoMode(QLineEdit::Password);
}

void luksaddkeyUI::partitionEntry(QString partition)
{
	ui.textEditPathToVolume->setText(partition);
	ui.textEditExistingPassphrase->clear();
	ui.textEditPassphraseToAdd->clear();
	ui.radioButtonNewPassphrase->setChecked(true);
	ui.radioButtonPassphraseinVolume->setChecked(true);
	ui.textEditPathToVolume->setFocus();
	ui.lineEditReEnterPassphrase->clear() ;
	this->show(); ;
}

void luksaddkeyUI::HideUI()
{
	this->hide();
}

void luksaddkeyUI::ShowUI()
{
	ui.textEditExistingPassphrase->clear();
	ui.textEditPassphraseToAdd->clear();
	ui.textEditPathToVolume->clear();
	ui.radioButtonNewPassphrase->setChecked(true);
	ui.radioButtonPassphraseinVolume->setChecked(true);
	ui.textEditPathToVolume->setFocus();
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
	HideUI() ;
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
	ui.lineEditReEnterPassphrase->setEnabled(true) ;
}

void luksaddkeyUI::rbNewPassphraseFromFile()
{
	ui.textEditPassphraseToAdd->setEchoMode(QLineEdit::Normal);
	ui.pushButtonOpenNewKeyFile->setEnabled(true);
	ui.labelNewPassphrase->setText(QString("key file")) ;
	ui.lineEditReEnterPassphrase->setEnabled(false) ;
	ui.lineEditReEnterPassphrase->clear() ;
}

void luksaddkeyUI::pbAdd(void)
{
	QString a = ui.textEditPathToVolume->text() ;
	QString b = ui.textEditExistingPassphrase->text() ;
	QString c = ui.textEditPassphraseToAdd->text() ;
	QString d = ui.lineEditReEnterPassphrase->text() ;

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
	if ( c != d ){
		m.setText(QString("ERROR: passphrases do not match"));
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
