/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
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
#include "../zuluCrypt-cli/executables.h"
#include "zulucrypt.h"
#include "miscfunctions.h"

#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>
#include <QIcon>

luksaddkeyUI::luksaddkeyUI(QWidget *parent) :
	QDialog(parent)
{
	ui = new Ui::luksaddkeyUI() ;
	ui->setupUi(this);
	ui->pushButtonOpenPartition->setIcon(QIcon(QString(":/partition.png")));
	ui->pushButtonOpenFile->setIcon(QIcon(QString(":/file.png")));

	lakt = NULL ;

	this->setFixedSize(this->size());

	pUI = new openpartition(this);
	pUI->setWindowFlags(Qt::Window | Qt::Dialog);

	connect(pUI,
		SIGNAL(clickedPartition(QString)),
		this,
		SLOT(partitionEntry(QString)));
	connect(this,
		SIGNAL(pbOpenPartitionClicked()),
		pUI,
		SLOT(ShowUI()));
	connect(ui->pushButtonOpenFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenFile())) ;
	connect(ui->pushButtonOpenExistingKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenExisitingKeyFile())) ;
	connect(ui->pushButtonOpenNewKeyFile,
		(SIGNAL(clicked())),
		this,
		SLOT(pbOpenNewKeyFile())) ;
	connect(ui->pushButtonOpenPartition,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenPartition(void))) ;
	connect(ui->pushButtonAdd,
		SIGNAL(clicked()),
		this,
		SLOT(pbAdd())) ;
	connect(ui->pushButtonCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancel())) ;
	connect(ui->radioButtonNewPassphrase,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbNewPassphrase())) ;
	connect(ui->radioButtonNewPassphraseFromFile,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbNewPassphraseFromFile())) ;
	connect(ui->radioButtonPassphraseinVolume,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbExistingPassphrase())) ;
	connect(ui->radioButtonPassphraseInVolumeFromFile,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbExistingPassphraseFromFile())) ;

	ui->lineEditReEnterPassphrase->setEchoMode(QLineEdit::Password);
}

void luksaddkeyUI::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if( lakt == NULL )
		pbCancel();
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
	emit HideUISignal(this);
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
	ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	ui->pushButtonOpenNewKeyFile->setEnabled(false);
	ui->textEditPathToVolume->setFocus();
	this->show();
}

void luksaddkeyUI::pbOpenExisitingKeyFile(void)
{	
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("existing key file"),
						 QDir::homePath(),
						 0);
	ui->textEditExistingPassphrase->setText( Z ) ;
}

void luksaddkeyUI::pbOpenNewKeyFile(void)
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("new key file"),
						 QDir::homePath(),
						 0);
	ui->textEditPassphraseToAdd->setText( Z ) ;
}

void luksaddkeyUI::pbOpenFile(void)
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("encrypted volume path"),
						 QDir::homePath(),
						 0);
	ui->textEditPathToVolume->setText( Z ) ;
}

void luksaddkeyUI::pbOpenPartition(void)
{
	emit pbOpenPartitionClicked() ;
}

void luksaddkeyUI::rbExistingPassphrase(void)
{
	ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Password);
	ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	ui->labelExistingPassphrase->setText(tr("passphrase")) ;
	ui->textEditExistingPassphrase->clear();
	ui->pushButtonOpenExistingKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
}

void luksaddkeyUI::rbExistingPassphraseFromFile(void)
{
	ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Normal);
	ui->pushButtonOpenExistingKeyFile->setEnabled(true);
	ui->labelExistingPassphrase->setText(tr("key file")) ;
	ui->textEditExistingPassphrase->clear();
	ui->pushButtonOpenExistingKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
}

void luksaddkeyUI::rbNewPassphrase(void)
{
	ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Password);
	ui->pushButtonOpenNewKeyFile->setEnabled(false);
	ui->labelNewPassphrase->setText(tr("passphrase")) ;
	ui->textEditPassphraseToAdd->clear();
	ui->lineEditReEnterPassphrase->setEnabled(true) ;
	ui->labelReEnterPassphrase->setEnabled(true);
	ui->pushButtonOpenNewKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
}

void luksaddkeyUI::rbNewPassphraseFromFile()
{
	ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Normal);
	ui->pushButtonOpenNewKeyFile->setEnabled(true);
	ui->labelNewPassphrase->setText(tr("key file")) ;
	ui->lineEditReEnterPassphrase->setEnabled(false) ;
	ui->lineEditReEnterPassphrase->clear() ;
	ui->labelReEnterPassphrase->setEnabled(false);
	ui->pushButtonOpenNewKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
}

void luksaddkeyUI::pbAdd(void)
{
	disableAll();
	volumePath = ui->textEditPathToVolume->text() ;
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
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("the encrypted volume path field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if( volumePath.mid(0,2) == QString("~/"))
		volumePath = QDir::homePath() + QString("/") + volumePath.mid(2) ;

	if( volumePath.mid(0,5) == QString("UUID=")){
		if( miscfunctions::isUUIDvalid(volumePath) == false ){
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("could not find any partition with the presented UUID"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			return ;
		}
	}
	if ( QFile::exists(volumePath) == false && volumePath.mid(0,5) != QString("UUID=")){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("volume path field does not point to a file or device"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if ( ExistingKey.isEmpty() == true ){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("existing passphrase field is empth"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if ( NewKey.isEmpty() == true ){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("new passphrase field is empty"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if ( ui->radioButtonNewPassphraseFromFile->isChecked() == false){
		m.setWindowTitle(tr("ERROR!"));
		if ( NewKey != d ){
			m.setText(tr("passphrases do not match"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			return ;
		}
	}
	volumePath = volumePath.replace("\"","\"\"\"") ;

	if ( miscfunctions::isLuks(volumePath) == false ){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("volume path does not point to a luks volume"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	QStringList l = miscfunctions::luksEmptySlots(volumePath) ;
	
	if( l.at(0) == l.at(1)){
		m.setWindowTitle(tr("ERROR!"));
		m.setText(tr("can not add any more keys, all slots are occupied"));
		m.addButton(QMessageBox::Ok);
		m.exec() ;
		enableAll();
		return ;
	}
	if(ui->radioButtonNewPassphraseFromFile->isChecked() == true){

		if( NewKey.mid(0,2) == QString("~/"))
			NewKey = QDir::homePath() + QString("/") + NewKey.mid(2) ;

		if(QFile::exists(NewKey) == false){
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("invalid path to a key file with a key to be added"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			return ;
		}
	}
	if(ui->radioButtonPassphraseInVolumeFromFile->isChecked() == true){

		if( ExistingKey.mid(0,2) == QString("~/"))
			ExistingKey = QDir::homePath() + QString("/") + ExistingKey.mid(2) ;

		if(QFile::exists(ExistingKey) == false){
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("invalid path to a key file with an existing key"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			enableAll();
			return ;
		}
	}
	QString existingPassType ;
	QString newPassType ;

	if ( x == true)
		existingPassType = QString(" -f ") ;
	else
		existingPassType = QString(" -p ") ;

	ExistingKey = ExistingKey.replace("\"","\"\"\"") ;
	
	if ( y == true)
		newPassType = QString(" -f ") ;
	else
		newPassType = QString(" -p ") ;

	NewKey = NewKey.replace("\"","\"\"\"") ;	

	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" addkey ") ;
	exe = exe + "\"" + volumePath + "\"" + existingPassType + "\"" + ExistingKey ;
	exe = exe + "\"" + newPassType + "\"" + NewKey + "\"" ;

	lakt = new runInThread(exe);

	connect(lakt,
		SIGNAL(finished(runInThread *,int)),
		this,
		SLOT(threadfinished(runInThread *,int))) ;
	lakt->start();
}

void luksaddkeyUI::threadfinished(runInThread * lakt,int status)
{
	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

	QString ss ;
	QStringList x ;

	lakt->wait() ;
	delete lakt ;
	lakt = NULL ;

	switch( status ){
		case 0 :
			x = miscfunctions::luksEmptySlots(volumePath);
			m.setWindowTitle(tr("SUCCESS"));
			ss = tr("key added successfully.\n") ;
			ss = ss + x.at(0) + QString(" / ") + x.at(1) + tr(" slots are now in use") ;
			m.setText(ss);
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			this->hide();
			return ;
			break ;
		case 1 :
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("presented key does not match any key in the volume"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			break ;
		case 2 :
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("could not open luks device"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			break ;
		case 9 :
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("could not open key file for reading, run out of memory"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			break ;
		case 11 :
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("could not find any partition with the presented UUID"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
			break ;	
		default:
			m.setWindowTitle(tr("ERROR!"));
			m.setText(tr("un unrecognized error has occured, key not added"));
			m.addButton(QMessageBox::Ok);
			m.exec() ;
	}
	enableAll();
}

void luksaddkeyUI::disableAll()
{
	ui->labelExistingPassphrase->setEnabled(false);
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
	ui->labelReEnterPassphrase->setEnabled(false) ;
}

void luksaddkeyUI::enableAll()
{
	ui->labelExistingPassphrase->setEnabled(true);
	ui->labelLuksVolume->setEnabled(true);
	ui->labelNewPassphrase->setEnabled(true);
	ui->groupBox->setEnabled(true);
	ui->groupBox_2->setEnabled(true);
	ui->textEditExistingPassphrase->setEnabled(true);
	ui->textEditPassphraseToAdd->setEnabled(true);
	ui->textEditPathToVolume->setEnabled(true);
	ui->labelNewPassphrase->setEnabled(true);
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
	if(ui->radioButtonNewPassphrase->isChecked() == true)
		ui->labelReEnterPassphrase->setEnabled(true) ;
	if(ui->radioButtonNewPassphraseFromFile->isChecked() == true)
		;
	else
		ui->lineEditReEnterPassphrase->setEnabled(true);

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
