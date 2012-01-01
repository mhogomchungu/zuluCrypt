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

luksaddkey::luksaddkey(QWidget *parent) :
	QDialog(parent)
{
	m_ui = new Ui::luksaddkey() ;
	m_ui->setupUi(this);
	m_ui->pushButtonOpenPartition->setIcon(QIcon(QString(":/partition.png")));
	m_ui->pushButtonOpenFile->setIcon(QIcon(QString(":/file.png")));

	m_lakt = NULL ;

	this->setFixedSize(this->size());

	connect(m_ui->pushButtonOpenFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenFile())) ;
	connect(m_ui->pushButtonOpenExistingKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenExisitingKeyFile())) ;
	connect(m_ui->pushButtonOpenNewKeyFile,
		(SIGNAL(clicked())),
		this,
		SLOT(pbOpenNewKeyFile())) ;
	connect(m_ui->pushButtonOpenPartition,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenPartition(void))) ;
	connect(m_ui->pushButtonAdd,
		SIGNAL(clicked()),
		this,
		SLOT(pbAdd())) ;
	connect(m_ui->pushButtonCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancel())) ;
	connect(m_ui->radioButtonNewPassphrase,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbNewPassphrase())) ;
	connect(m_ui->radioButtonNewPassphraseFromFile,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbNewPassphraseFromFile())) ;
	connect(m_ui->radioButtonPassphraseinVolume,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbExistingPassphrase())) ;
	connect(m_ui->radioButtonPassphraseInVolumeFromFile,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbExistingPassphraseFromFile())) ;

	m_ui->lineEditReEnterPassphrase->setEchoMode(QLineEdit::Password);
}

void luksaddkey::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if( m_lakt == NULL )
		pbCancel();
}

void luksaddkey::partitionEntry(QString partition)
{
	enableAll();
	m_ui->textEditPathToVolume->setText(partition);
	m_ui->textEditExistingPassphrase->clear();
	m_ui->textEditPassphraseToAdd->clear();
	m_ui->radioButtonNewPassphrase->setChecked(true);
	m_ui->radioButtonPassphraseinVolume->setChecked(true);
	m_ui->textEditExistingPassphrase->setFocus();
	m_ui->lineEditReEnterPassphrase->clear() ;
	this->show(); ;
}

void luksaddkey::HideUI()
{
	this->hide();
	emit HideUISignal(this);
}

void luksaddkey::ShowUI()
{
	enableAll();
	m_ui->textEditExistingPassphrase->clear();
	m_ui->textEditPassphraseToAdd->clear();
	m_ui->textEditPathToVolume->clear();
	m_ui->lineEditReEnterPassphrase->clear();
	m_ui->radioButtonNewPassphrase->setChecked(true);
	m_ui->radioButtonPassphraseinVolume->setChecked(true);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(false);
	m_ui->textEditPathToVolume->setFocus();
	this->show();
}

void luksaddkey::pbOpenExisitingKeyFile(void)
{	
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("existing key file"),
						 QDir::homePath(),
						 0);
	m_ui->textEditExistingPassphrase->setText( Z ) ;
}

void luksaddkey::pbOpenNewKeyFile(void)
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("new key file"),
						 QDir::homePath(),
						 0);
	m_ui->textEditPassphraseToAdd->setText( Z ) ;
}

void luksaddkey::pbOpenFile(void)
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("encrypted volume path"),
						 QDir::homePath(),
						 0);
	m_ui->textEditPathToVolume->setText( Z ) ;
}

void luksaddkey::pbOpenPartition(void)
{
	openpartition * openPartition = new openpartition(this);
	openPartition->setWindowFlags(Qt::Window | Qt::Dialog);
	openPartition->setFont(this->font());

	connect(openPartition,
		SIGNAL(clickedPartition(QString)),
		this,
		SLOT(partitionEntry(QString)));
	connect(openPartition,
		SIGNAL(HideUISignal(openpartition*)),
		this,
		SLOT(openpartitionFinished(openpartition*)));
	openPartition->ShowAllPartitions();
}

void luksaddkey::openpartitionFinished(openpartition *obj)
{
	obj->deleteLater();
}

void luksaddkey::rbExistingPassphrase(void)
{
	m_ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Password);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(false);
	m_ui->labelExistingPassphrase->setText(tr("passphrase")) ;
	m_ui->textEditExistingPassphrase->clear();
	m_ui->pushButtonOpenExistingKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
}

void luksaddkey::rbExistingPassphraseFromFile(void)
{
	m_ui->textEditExistingPassphrase->setEchoMode(QLineEdit::Normal);
	m_ui->pushButtonOpenExistingKeyFile->setEnabled(true);
	m_ui->labelExistingPassphrase->setText(tr("key file")) ;
	m_ui->textEditExistingPassphrase->clear();
	m_ui->pushButtonOpenExistingKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
}

void luksaddkey::rbNewPassphrase(void)
{
	m_ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Password);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(false);
	m_ui->labelNewPassphrase->setText(tr("passphrase")) ;
	m_ui->textEditPassphraseToAdd->clear();
	m_ui->lineEditReEnterPassphrase->setEnabled(true) ;
	m_ui->labelReEnterPassphrase->setEnabled(true);
	m_ui->pushButtonOpenNewKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
}

void luksaddkey::rbNewPassphraseFromFile()
{
	m_ui->textEditPassphraseToAdd->setEchoMode(QLineEdit::Normal);
	m_ui->pushButtonOpenNewKeyFile->setEnabled(true);
	m_ui->labelNewPassphrase->setText(tr("key file")) ;
	m_ui->lineEditReEnterPassphrase->setEnabled(false) ;
	m_ui->lineEditReEnterPassphrase->clear() ;
	m_ui->labelReEnterPassphrase->setEnabled(false);
	m_ui->pushButtonOpenNewKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
}

void luksaddkey::pbAdd(void)
{
	m_volumePath = m_ui->textEditPathToVolume->text() ;
	QString ExistingKey = m_ui->textEditExistingPassphrase->text() ;
	QString NewKey = m_ui->textEditPassphraseToAdd->text() ;
	QString NewKey_1 = m_ui->lineEditReEnterPassphrase->text() ;

	if ( m_volumePath == QString("") ){
		UIMessage(tr("ERROR!"),tr("the encrypted volume path field is empty"));
		return ;
	}
	if( m_volumePath.mid(0,2) == QString("~/"))
		m_volumePath = QDir::homePath() + QString("/") + m_volumePath.mid(2) ;

	if( m_volumePath.mid(0,5) == QString("UUID=")){
		if( miscfunctions::isUUIDvalid(m_volumePath) == false ){
			UIMessage(tr("ERROR!"),tr("could not find any partition with the presented UUID"));
			enableAll();
			return ;
		}
	}
	if ( miscfunctions::exists(m_volumePath) == false && m_volumePath.mid(0,5) != QString("UUID=")){
		UIMessage(tr("ERROR!"),tr("volume path field does not point to a file or device"));
		return ;
	}
	if ( ExistingKey == QString("") ){
		UIMessage(tr("ERROR!"),tr("existing passphrase field is empth"));
		m_ui->textEditExistingPassphrase->setFocus();
		return ;
	}
	if ( NewKey.isEmpty() == true ){
		UIMessage(tr("ERROR!"),tr("new passphrase field is empty"));
		m_ui->textEditPassphraseToAdd->setFocus();
		return ;
	}
	if ( m_ui->radioButtonNewPassphraseFromFile->isChecked() == false){
		if ( NewKey != NewKey_1 ){
			UIMessage(tr("ERROR!"),tr("passphrases do not match"));
			return ;
		}
	}
	m_volumePath.replace("\"","\"\"\"") ;

	if ( miscfunctions::isLuks(m_volumePath) == false ){
		UIMessage(tr("ERROR!"),tr("volume path does not point to a luks volume"));
		return ;
	}
	QStringList l = miscfunctions::luksEmptySlots(m_volumePath) ;
	
	if( l.at(0) == l.at(1)){
		UIMessage(tr("ERROR!"),tr("can not add any more keys, all slots are occupied"));
		return ;
	}
	if(m_ui->radioButtonNewPassphraseFromFile->isChecked() == true){
		if( NewKey.mid(0,2) == QString("~/"))
			NewKey = QDir::homePath() + QString("/") + NewKey.mid(2) ;

		if(miscfunctions::exists(NewKey) == false){
			UIMessage(tr("ERROR!"),tr("invalid path to a key file with a key to be added"));
			return ;
		}
	}
	if(m_ui->radioButtonPassphraseInVolumeFromFile->isChecked() == true){
		if( ExistingKey.mid(0,2) == QString("~/"))
			ExistingKey = QDir::homePath() + QString("/") + ExistingKey.mid(2) ;

		if(miscfunctions::exists(ExistingKey) == false){
			UIMessage(tr("ERROR!"),tr("invalid path to a key file with an existing key"));
			return ;
		}
	}
	QString existingPassType ;
	QString newPassType ;

	bool x = m_ui->radioButtonPassphraseInVolumeFromFile->isChecked() ;
	bool y = m_ui->radioButtonNewPassphraseFromFile->isChecked() ;

	if ( x == true)
		existingPassType = QString(" -f ") ;
	else
		existingPassType = QString(" -p ") ;

	ExistingKey.replace("\"","\"\"\"") ;
	
	if ( y == true)
		newPassType = QString(" -f ") ;
	else
		newPassType = QString(" -p ") ;

	NewKey.replace("\"","\"\"\"") ;

	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" addkey ") ;
	exe = exe + "\"" + m_volumePath + "\"" + existingPassType + "\"" + ExistingKey ;
	exe = exe + "\"" + newPassType + "\"" + NewKey + "\"" ;

	disableAll();

	m_lakt = new runInThread(exe);

	connect(m_lakt,
		SIGNAL(finished(runInThread *,int)),
		this,
		SLOT(threadfinished(runInThread *,int))) ;
	m_lakt->start();
}

void luksaddkey::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}

void luksaddkey::threadfinished(runInThread * lakt,int status)
{
	QStringList x ;
	lakt->deleteLater(); ;
	lakt = NULL ;
	QString success;
	switch( status ){
		case 0 :
			x = miscfunctions::luksEmptySlots(m_volumePath);
			success = tr("key added successfully.\n%1 / %2 slots are now in use").arg(x.at(0)).arg(x.at(1));
			UIMessage(tr("SUCCESS"),success);
			HideUI();
			return ;
		case 1 :
			UIMessage(tr("ERROR!"),tr("presented key does not match any key in the volume"));
			break ;
		case 2 :
			UIMessage(tr("ERROR!"),tr("could not open luks device"));
			break ;
		case 9 :
			UIMessage(tr("ERROR!"),tr("could not open key file for reading, run out of memory"));
			break ;
		case 11 :
			UIMessage(tr("ERROR!"),tr("presented UUID does not match any UUID from attached partitions"));
			break ;	
		default:
			UIMessage(tr("ERROR!"),tr("un unrecognized error has occured, key not added"));
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
	if(m_ui->radioButtonNewPassphraseFromFile->isChecked() == true)
		;
	else
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
