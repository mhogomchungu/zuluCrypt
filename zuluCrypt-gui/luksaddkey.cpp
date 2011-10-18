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
#include "executables.h"
#include "zulucrypt.h"

#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>

luksaddkeyUI::luksaddkeyUI(QWidget *parent) :
	QDialog(parent)
{
	ui = new Ui::luksaddkeyUI() ;

	ui->setupUi(this);

	this->setFixedSize(this->size());

	pUI = new openpartition(this);
	pUI->setWindowFlags(Qt::Window | Qt::Dialog);

	connect(pUI,SIGNAL(clickedPartition(QString)),this,SLOT(partitionEntry(QString)));
	connect(this,SIGNAL(pbOpenPartitionClicked()),pUI,SLOT(ShowUI()));
	connect(ui->pushButtonOpenFile,SIGNAL(clicked()), this,SLOT(pbOpenFile())) ;

	connect(ui->pushButtonOpenExistingKeyFile,SIGNAL(clicked()),this, SLOT(pbOpenExisitingKeyFile())) ;

	connect(ui->pushButtonOpenNewKeyFile,(SIGNAL(clicked())),this, SLOT(pbOpenNewKeyFile())) ;

	connect(ui->pushButtonOpenPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition(void))) ;

	connect(ui->pushButtonAdd,SIGNAL(clicked()), this, SLOT(pbAdd())) ;

	connect(ui->pushButtonCancel,SIGNAL(clicked()),SLOT(pbCancel())) ;

	connect(ui->radioButtonNewPassphrase,SIGNAL(toggled(bool)),SLOT(rbNewPassphrase())) ;

	connect(ui->radioButtonNewPassphraseFromFile,SIGNAL(toggled(bool)),SLOT(rbNewPassphraseFromFile())) ;

	connect(ui->radioButtonPassphraseinVolume,SIGNAL(toggled(bool)),SLOT(rbExistingPassphrase())) ;

	connect(ui->radioButtonPassphraseInVolumeFromFile,SIGNAL(toggled(bool)),SLOT(rbExistingPassphraseFromFile())) ;

	ui->lineEditReEnterPassphrase->setEchoMode(QLineEdit::Password);
}

void luksaddkeyUI::partitionEntry(QString partition)
{
	enableAll();

	ui->textEditPathToVolume->setText(partition);
	ui->textEditExistingPassphrase->clear();
	ui->textEditPassphraseToAdd->clear();
	ui->radioButtonNewPassphrase->setChecked(true);
	ui->radioButtonPassphraseinVolume->setChecked(true);
	ui->textEditExistingPassphrase->setFocus();
	ui->lineEditReEnterPassphrase->clear() ;
	this->show(); ;
}

void luksaddkeyUI::HideUI()
{
	this->hide();
}

void luksaddkeyUI::ShowUI()
{
	enableAll();

	ui->textEditExistingPassphrase->clear();
	ui->textEditPassphraseToAdd->clear();
	ui->textEditPathToVolume->clear();
	ui->lineEditReEnterPassphrase->clear();
	ui->radioButtonNewPassphrase->setChecked(true);
	ui->radioButtonPassphraseinVolume->setChecked(true);
	ui->textEditPathToVolume->setFocus();
	this->show();
}

void luksaddkeyUI::pbOpenExisitingKeyFile(void)
{	
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("existing key file"),QDir::homePath(),0);
	ui->textEditExistingPassphrase->setText( Z ) ;
}

void luksaddkeyUI::pbOpenNewKeyFile(void)
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("new key file"),QDir::homePath(),0);
	ui->textEditPassphraseToAdd->setText( Z ) ;
}

void luksaddkeyUI::pbOpenFile(void)
{
	QString Z = QFileDialog::getOpenFileName((QWidget *) this,QString("encrypted volume path"),QDir::homePath(),0);
	ui->textEditPathToVolume->setText( Z ) ;
}

void luksaddkeyUI::pbOpenPartition(void)
{
	//HideUI() ;
	emit pbOpenPartitionClicked() ;
}

void luksaddkeyUI::rbExistingPassphrase(void)
{
	ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Password);
	ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	ui->labelExistingPassphrase->setText(QString("passphrase")) ;
	ui->textEditExistingPassphrase->clear();
}

void luksaddkeyUI::rbExistingPassphraseFromFile(void)
{
	ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Normal);
	ui->pushButtonOpenExistingKeyFile->setEnabled(true);
	ui->labelExistingPassphrase->setText(QString("key file")) ;
	ui->textEditExistingPassphrase->clear();

}

void luksaddkeyUI::rbNewPassphrase(void)
{
	ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Password);
	ui->pushButtonOpenNewKeyFile->setEnabled(false);
	ui->labelNewPassphrase->setText(QString("passphrase")) ;
	ui->textEditPassphraseToAdd->clear();
	ui->lineEditReEnterPassphrase->setEnabled(true) ;
}

void luksaddkeyUI::rbNewPassphraseFromFile()
{
	ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Normal);
	ui->pushButtonOpenNewKeyFile->setEnabled(true);
	ui->labelNewPassphrase->setText(QString("key file")) ;
	ui->lineEditReEnterPassphrase->setEnabled(false) ;
	ui->lineEditReEnterPassphrase->clear() ;
}

void luksaddkeyUI::pbAdd(void)
{
	disableAll();

	QString volumePath = ui->textEditPathToVolume->text() ;
	QString ExistingKey = ui->textEditExistingPassphrase->text() ;
	QString NewKey = ui->textEditPassphraseToAdd->text() ;
	QString d = ui->lineEditReEnterPassphrase->text() ;

	bool x = ui->radioButtonPassphraseInVolumeFromFile->isChecked() ;
	bool y = ui->radioButtonNewPassphraseFromFile->isChecked() ;

	QMessageBox m ;	
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	if ( volumePath.isEmpty() == true ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("the encrypted volume path field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if ( ExistingKey.isEmpty() == true ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("existing passphrase field is empth"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if ( NewKey.isEmpty() == true ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("new passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if ( ui->radioButtonNewPassphraseFromFile->isChecked() == false){
		m.setWindowTitle(QString("ERROR!"));
		if ( NewKey != d ){
			m.setText(QString("passphrases do not match"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			return ;
		}
	}

	if ( zuluCrypt::isLuks(volumePath) == false ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("volume path does not point to a luks volume"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}

	if( zuluCrypt::luksEmptySlots(volumePath) == '8'){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("can not add any more keys, all slots are occupied"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}

	QString existingPassType ;
	QString newPassType ;

	if ( x == true)
		existingPassType = QString(" -f ") ;
	else{
		existingPassType = QString(" -p ") ;

		for( int i = 0 ; i < ExistingKey.size() ; i++){

			if( ExistingKey.at(i).toAscii() == '\"'){
				ExistingKey.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	if ( y == true)
		newPassType = QString(" -f ") ;
	else{
		newPassType = QString(" -p ") ;

		for( int i = 0 ; i < NewKey.size() ; i++){

			if( NewKey.at(i).toAscii() == '\"'){
				NewKey.insert(i,QString("\"\""));
				i = i + 2 ;
			}
		}
	}

	QString exe = QString(ZULUCRYPTzuluCrypt) + QString(" addkey ") + "\"" + volumePath + "\"" + existingPassType + "\"" + ExistingKey + "\"" + newPassType + "\"" + NewKey + "\"" ;

	lakt = new luksAddKeyThread(exe,&status);

	connect(lakt,SIGNAL(finished()),this,SLOT(threadfinished())) ;

	lakt->start();
}

void luksaddkeyUI::threadfinished()
{
	delete lakt ;

	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	switch( status ){
		case 0 :{
			m.setWindowTitle(QString("SUCCESS"));
			QString ss = QString("key added successfully\n") + QString(zuluCrypt::luksEmptySlots(ui->textEditPathToVolume->text())) + QString(" / 8 slots are now in use") ;
			m.setText(ss);
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			this->hide();
			return ;
			}break ;
		case 1 :
			m.setWindowTitle(QString("ERROR!"));
			m.setText(QString("presented key does not match any key in the volume"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			break ;
		case 2 :
			m.setWindowTitle(QString("ERROR!"));
			m.setText(QString("could not open luks device"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			break ;
		case 4 :
			m.setWindowTitle(QString("ERROR!"));
			m.setText(QString("couldnt find cryptsetup.so library in /usr/local/lib,/usr/lib and /lib"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			break ;
		case 9 :
			m.setWindowTitle(QString("ERROR!"));
			m.setText(QString("could not open key file for reading, run out of memory"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			break ;

		default:
			m.setWindowTitle(QString("ERROR!"));
			m.setText(QString("un unrecognized error has occured, key not added"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();

	}
}

void luksaddkeyUI::disableAll()
{
	ui->labelExistingPassphrase->setEnabled(false);
	ui->label->setEnabled(false);
	ui->labelLuksVolume->setEnabled(false);
	ui->labelNewPassphrase->setEnabled(false);
	ui->groupBox->setEnabled(false);
	ui->groupBox_2->setEnabled(false);
	ui->textEditExistingPassphrase->setEnabled(false);
	ui->textEditPassphraseToAdd->setEnabled(false);
	ui->textEditPathToVolume->setEnabled(false);
	ui->lineEditReEnterPassphrase->setEnabled(false);
	ui->labelNewPassphrase->setEnabled(false);
	ui->lineEditReEnterPassphrase->setEnabled(false);
	ui->pushButtonAdd->setEnabled(false);
	ui->pushButtonCancel->setEnabled(false);
	ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	ui->pushButtonOpenFile->setEnabled(false);
	ui->pushButtonOpenNewKeyFile->setEnabled(false);
	ui->pushButtonOpenPartition->setEnabled(false);
	ui->radioButtonNewPassphrase->setEnabled(false);
	ui->radioButtonNewPassphraseFromFile->setEnabled(false);
	ui->radioButtonPassphraseinVolume->setEnabled(false);
	ui->radioButtonPassphraseInVolumeFromFile->setEnabled(false);
}

void luksaddkeyUI::enableAll()
{
	ui->labelExistingPassphrase->setEnabled(true);
	ui->label->setEnabled(true);
	ui->labelLuksVolume->setEnabled(true);
	ui->labelNewPassphrase->setEnabled(true);
	ui->groupBox->setEnabled(true);
	ui->groupBox_2->setEnabled(true);
	ui->textEditExistingPassphrase->setEnabled(true);
	ui->textEditPassphraseToAdd->setEnabled(true);
	ui->textEditPathToVolume->setEnabled(true);
	ui->lineEditReEnterPassphrase->setEnabled(true);
	ui->labelNewPassphrase->setEnabled(true);
	ui->lineEditReEnterPassphrase->setEnabled(true);
	ui->pushButtonAdd->setEnabled(true);
	ui->pushButtonCancel->setEnabled(true);
	ui->pushButtonOpenExistingKeyFile->setEnabled(true);
	ui->pushButtonOpenFile->setEnabled(true);
	ui->pushButtonOpenNewKeyFile->setEnabled(true);
	ui->pushButtonOpenPartition->setEnabled(true);
	ui->radioButtonNewPassphrase->setEnabled(true);
	ui->radioButtonNewPassphraseFromFile->setEnabled(true);
	ui->radioButtonPassphraseinVolume->setEnabled(true);
	ui->radioButtonPassphraseInVolumeFromFile->setEnabled(true);
}

void luksaddkeyUI::pbCancel(void)
{
	HideUI() ;
}

luksaddkeyUI::~luksaddkeyUI()
{
	delete pUI ;
	delete ui ;
}
