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
#include "../zuluCrypt-cli/constants.h"
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

luksaddkey::luksaddkey(QWidget *parent) :
	QDialog(parent)
{
	m_ui = new Ui::luksaddkey() ;
	m_ui->setupUi(this);

	m_isWindowClosable = true ;

	m_ui->textEditPathToVolume->setText(QString(""));

	m_ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Password);
	m_ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Password);

	m_msg.setParent(this);

	this->setFixedSize(this->size());
	this->setWindowFlags(Qt::Window | Qt::Dialog);
	this->setFont(parent->font());

	connect(m_ui->pushButtonOpenFile,SIGNAL(clicked()),this,SLOT(pbOpenFile())) ;
	connect(m_ui->pushButtonOpenExistingKeyFile,SIGNAL(clicked()),this,SLOT(pbOpenExisitingKeyFile())) ;
	connect(m_ui->pushButtonOpenNewKeyFile,SIGNAL(clicked()),this,SLOT(pbOpenNewKeyFile())) ;
	connect(m_ui->pushButtonOpenPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition(void))) ;
	connect(m_ui->pushButtonAdd,SIGNAL(clicked()),this,SLOT(pbAdd())) ;
	connect(m_ui->pushButtonCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(m_ui->radioButtonNewPassphrase,SIGNAL(toggled(bool)),this,SLOT(rbNewPassphrase())) ;
	connect(m_ui->radioButtonNewPassphraseFromFile,SIGNAL(toggled(bool)),this,SLOT(rbNewPassphraseFromFile())) ;
	connect(m_ui->radioButtonPassphraseinVolume,SIGNAL(toggled(bool)),this,SLOT(rbExistingPassphrase())) ;
	connect(m_ui->radioButtonPassphraseInVolumeFromFile,SIGNAL(toggled(bool)),this,SLOT(rbExistingPassphraseFromFile())) ;

	m_ui->lineEditReEnterPassphrase->setEchoMode(QLineEdit::Password);

	m_ui->radioButtonNewPassphrase->setChecked(true);
	m_ui->radioButtonPassphraseinVolume->setChecked(true);

	m_ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(false);

	m_ui->pushButtonOpenPartition->setIcon(QIcon(QString(":/partition.png")));
	m_ui->pushButtonOpenFile->setIcon(QIcon(QString(":/file.png")));
}

void luksaddkey::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if( m_isWindowClosable == true )
		HideUI();
}

void luksaddkey::ShowUI(QString path)
{
	m_ui->textEditPathToVolume->setText(path);
	this->ShowUI();
}

void luksaddkey::ShowUI()
{
	if( m_ui->textEditPathToVolume->text() == QString("") )
		m_ui->textEditPathToVolume->setFocus();
	else
		m_ui->textEditExistingPassphrase->setFocus();
	this->show();
}

void luksaddkey::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void luksaddkey::pbOpenExisitingKeyFile(void)
{	
	QString Z = QFileDialog::getOpenFileName(this,tr("existing key file"),QDir::homePath(),0);
	m_ui->textEditExistingPassphrase->setText( Z ) ;
}

void luksaddkey::pbOpenNewKeyFile(void)
{
	QString Z = QFileDialog::getOpenFileName(this,tr("new key file"),QDir::homePath(),0);
	m_ui->textEditPassphraseToAdd->setText( Z ) ;
}

void luksaddkey::pbOpenFile(void)
{
	QString Z = QFileDialog::getOpenFileName(this,tr("encrypted volume path"),QDir::homePath(),0);
	m_ui->textEditPathToVolume->setText( Z ) ;
}

void luksaddkey::pbOpenPartition(void)
{
	openpartition * openPartition = new openpartition(this);
	connect(openPartition,SIGNAL(clickedPartition(QString)),this,SLOT(ShowUI(QString)));
	connect(openPartition,SIGNAL(HideUISignal()),openPartition,SLOT(deleteLater()));
	openPartition->ShowAllPartitions();
}

void luksaddkey::rbExistingPassphrase(void)
{
	m_ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Password);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	m_ui->labelExistingPassphrase->setText(tr("key")) ;
	m_ui->textEditExistingPassphrase->clear();
	m_ui->pushButtonOpenExistingKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
	m_ui->textEditExistingPassphrase->setFocus();
}

void luksaddkey::rbExistingPassphraseFromFile(void)
{
	m_ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Normal);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(true);
	m_ui->labelExistingPassphrase->setText(tr("keyfile")) ;
	m_ui->textEditExistingPassphrase->clear();
	m_ui->pushButtonOpenExistingKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
	m_ui->textEditExistingPassphrase->setFocus();

}

void luksaddkey::rbNewPassphrase(void)
{
	m_ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Password);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(false);
	m_ui->labelNewPassphrase->setText(tr("key")) ;
	m_ui->textEditPassphraseToAdd->clear();
	m_ui->lineEditReEnterPassphrase->setEnabled(true) ;
	m_ui->labelReEnterPassphrase->setEnabled(true);
	m_ui->pushButtonOpenNewKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
	m_ui->textEditPassphraseToAdd->setFocus();
}

void luksaddkey::rbNewPassphraseFromFile()
{
	m_ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Normal);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(true);
	m_ui->labelNewPassphrase->setText(tr("keyfile")) ;
	m_ui->lineEditReEnterPassphrase->setEnabled(false) ;
	m_ui->lineEditReEnterPassphrase->clear() ;
	m_ui->labelReEnterPassphrase->setEnabled(false);
	m_ui->pushButtonOpenNewKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
	m_ui->textEditPassphraseToAdd->setFocus();
}

void luksaddkey::pbAdd(void)
{
	m_volumePath = miscfunctions::resolveHomeSymbol(m_ui->textEditPathToVolume->text()) ;
	QString ExistingKey = m_ui->textEditExistingPassphrase->text() ;

	QString NewKey = m_ui->textEditPassphraseToAdd->text() ;
	QString NewKey_1 = m_ui->lineEditReEnterPassphrase->text() ;

	if(m_ui->radioButtonNewPassphraseFromFile->isChecked()){
		if( m_volumePath.isEmpty() || ExistingKey.isEmpty() || NewKey.isEmpty() )
			return m_msg.UIMessage(tr("ERROR!"),tr("atleast one required field is empty"));
	}else{
		if( m_volumePath.isEmpty() || ExistingKey.isEmpty() || NewKey.isEmpty() || NewKey_1.isEmpty())
			return m_msg.UIMessage(tr("ERROR!"),tr("atleast one required field is empty"));
	}

	m_volumePath.replace("\"","\"\"\"") ;
	ExistingKey.replace("\"","\"\"\"") ;
	NewKey.replace("\"","\"\"\"") ;

	QString existingPassType ;

	if (m_ui->radioButtonPassphraseInVolumeFromFile->isChecked()){
		ExistingKey = miscfunctions::resolveHomeSymbol(ExistingKey);
		existingPassType = QString("-u") ;
	}else
		existingPassType = QString("-y") ;

	QString newPassType ;
	if (m_ui->radioButtonNewPassphraseFromFile->isChecked()){
		NewKey = miscfunctions::resolveHomeSymbol(NewKey);
		newPassType = QString("-n") ;
	}else
		newPassType = QString("-l") ;

	QString a = QString(ZULUCRYPTzuluCrypt) ;
	QString b = m_volumePath ;
	QString c = existingPassType ;
	QString d = ExistingKey ;
	QString e = newPassType ;
	QString f = NewKey ;

	QString exe = QString("%1 -a -d \"%2\" %3 \"%4\" %5 \"%6\"").arg(a).arg(b).arg(c).arg(d).arg(e).arg(f);

	m_isWindowClosable = false ;

	disableAll();

	runInThread * lakt = new runInThread(exe);

	connect(lakt,SIGNAL(finished(int)),this,SLOT(threadfinished(int))) ;
	QThreadPool::globalInstance()->start(lakt);
}

void luksaddkey::threadfinished(int status)
{
	m_isWindowClosable = true ;
	QStringList x ;
	QString success;
	switch( status ){
		case 0 :
			x = miscfunctions::luksEmptySlots(m_volumePath);
			success = tr("key added successfully.\n%1 / %2 slots are now in use").arg(x.at(0)).arg(x.at(1));
			m_msg.UIMessage(tr("SUCCESS!"),success);
			HideUI();
			return ;			
		case 1  : m_msg.UIMessage(tr("ERROR!"),tr("presented key does not match any key in the volume" )) ;          	break ;
		case 2  : m_msg.UIMessage(tr("ERROR!"),tr("could not open luks device" )) ;		                      	break ;
		case 4  : m_msg.UIMessage(tr("ERROR!"),tr("device does not exist" )) ;	                                      	break ;
		case 5  : m_msg.UIMessage(tr("ERROR!"),tr("wrong arguments" )) ;	                                      	break ;
		case 6  : m_msg.UIMessage(tr("ERROR!"),tr("one or more required argument(s) for this operation is missing" ));	break ;
		case 7  : m_msg.UIMessage(tr("ERROR!"),tr("new passphrases do not match" ) );	                              	break ;
		case 8  : m_msg.UIMessage(tr("ERROR!"),tr("one or both keyfile(s) does not exist" )) ;	                      	break ;
		case 9  : m_msg.UIMessage(tr("ERROR!"),tr("couldnt get enought memory to hold the key file" )) ;	     	break ;
		case 10 : m_msg.UIMessage(tr("ERROR!"),tr("all key slots are occupied, can not add any more keys" )) ;	      	break ;
		case 11 : m_msg.UIMessage(tr("ERROR!"),tr("insufficient privilege to write to the volume" )) ;		        break ;
		case 12 : m_msg.UIMessage(tr("ERROR!"),tr("insufficient privilege to open key file for reading" ));		break ;
		case 13 : m_msg.UIMessage(tr("ERROR!"),tr("only root user can add keys to system devices" ));			break ;
		case 110: m_msg.UIMessage(tr("ERROR!"),tr("can not find a partition that match presented UUID" ));		break ;
		default : m_msg.UIMessage(tr("ERROR!"),tr("unrecognized ERROR! with status number %1 encountered").arg( status ));
	}
	enableAll();
}

void luksaddkey::disableAll()
{
	m_ui->labelExistingPassphrase->setEnabled(false);
	m_ui->labelLuksVolume->setEnabled(false);
	m_ui->labelNewPassphrase->setEnabled(false);
	m_ui->groupBox->setEnabled(false);
	m_ui->groupBox_2->setEnabled(false);
	m_ui->textEditExistingPassphrase->setEnabled(false);
	m_ui->textEditPassphraseToAdd->setEnabled(false);
	m_ui->textEditPathToVolume->setEnabled(false);
	m_ui->lineEditReEnterPassphrase->setEnabled(false);
	m_ui->labelNewPassphrase->setEnabled(false);
	m_ui->lineEditReEnterPassphrase->setEnabled(false);
	m_ui->pushButtonAdd->setEnabled(false);
	m_ui->pushButtonCancel->setEnabled(false);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	m_ui->pushButtonOpenFile->setEnabled(false);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(false);
	m_ui->pushButtonOpenPartition->setEnabled(false);
	m_ui->radioButtonNewPassphrase->setEnabled(false);
	m_ui->radioButtonNewPassphraseFromFile->setEnabled(false);
	m_ui->radioButtonPassphraseinVolume->setEnabled(false);
	m_ui->radioButtonPassphraseInVolumeFromFile->setEnabled(false);
	m_ui->labelReEnterPassphrase->setEnabled(false) ;
}

void luksaddkey::enableAll()
{
	m_ui->labelExistingPassphrase->setEnabled(true);
	m_ui->labelLuksVolume->setEnabled(true);
	m_ui->labelNewPassphrase->setEnabled(true);
	m_ui->groupBox->setEnabled(true);
	m_ui->groupBox_2->setEnabled(true);
	m_ui->textEditExistingPassphrase->setEnabled(true);
	m_ui->textEditPassphraseToAdd->setEnabled(true);
	m_ui->textEditPathToVolume->setEnabled(true);
	m_ui->labelNewPassphrase->setEnabled(true);
	m_ui->pushButtonAdd->setEnabled(true);
	m_ui->pushButtonCancel->setEnabled(true);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(true);
	m_ui->pushButtonOpenFile->setEnabled(true);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(true);
	m_ui->pushButtonOpenPartition->setEnabled(true);
	m_ui->radioButtonNewPassphrase->setEnabled(true);
	m_ui->radioButtonNewPassphraseFromFile->setEnabled(true);
	m_ui->radioButtonPassphraseinVolume->setEnabled(true);
	m_ui->radioButtonPassphraseInVolumeFromFile->setEnabled(true);
	if(m_ui->radioButtonNewPassphrase->isChecked() == true)
		m_ui->labelReEnterPassphrase->setEnabled(true) ;
	if(m_ui->radioButtonNewPassphraseFromFile->isChecked() == false)
		m_ui->lineEditReEnterPassphrase->setEnabled(true);
}

void luksaddkey::pbCancel(void)
{
	HideUI() ;
}

luksaddkey::~luksaddkey()
{
	delete m_ui ;
}
