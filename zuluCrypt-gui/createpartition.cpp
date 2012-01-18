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
#include "miscfunctions.h"
#include "createpartition.h"
#include "ui_createpartition.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QAbstractButton>
#include <QProcess>
#include <QThread>

#include "../zuluCrypt-cli/executables.h"

createpartition::createpartition(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::createpartition)
{
	m_ui->setupUi(this);
	this->setFixedSize(this->size());
	m_ui->lineEditVolumePath->setEnabled(false);
	m_ui->lineEditPassphrase1->setFocus();
	m_cvt = NULL ;

	connect(m_ui->pbOpenKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenKeyFile()));
	connect(m_ui->pbCreate,
		SIGNAL(clicked()),
		this,
		SLOT(pbCreateClicked()));
	connect(m_ui->pbCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancelClicked()));
	connect(m_ui->rbPassphrase,
		SIGNAL(clicked()),
		this,
		SLOT(rbPassphraseClicked()));
	connect(m_ui->rbPassphraseFromFile,
		SIGNAL(clicked()),
		this,
		SLOT(rbPasssphraseFromFileClicked()));
	connect(m_ui->comboBoxVolumeType,
		SIGNAL(currentIndexChanged(int)),
		this,
		SLOT(rng(int))) ;
}

void createpartition::rng(int s)
{
	if( s == 1)
		m_ui->comboBoxRNG->setEnabled(false);
	else
		m_ui->comboBoxRNG->setEnabled(true);
}

void createpartition::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if(m_cvt == NULL )
		pbCancelClicked() ;
}

void createpartition::ShowPartition(QString volume)
{
	ShowUI(tr("path to partition"),volume);
}

void createpartition::ShowFile(QString volume)
{
	ShowUI(tr("path to file"),volume);
}

void::createpartition::ShowUI(QString l,QString v)
{
	enableAll();
	m_ui->labelVolumePath->setText(l);
	m_ui->lineEditVolumePath->setText(v);
	m_ui->rbPassphrase->setChecked(true);
	m_ui->pbOpenKeyFile->setEnabled(false);
	m_ui->lineEditVolumePath->setEnabled(false);
	this->rbPassphraseClicked() ;
	m_ui->lineEditPassphrase1->setFocus();
	m_ui->comboBoxFS->setCurrentIndex(0);
	m_ui->comboBoxRNG->setCurrentIndex(0);
	m_ui->comboBoxVolumeType->setCurrentIndex(0);
	m_ui->labelRepeatPassPhrase->setEnabled(true);
	m_created = false ;
	this->show();
}

void createpartition::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("key file path"),
						 QDir::homePath(),
						 0);
	m_ui->lineEditPassphrase1->setText(Z);
}

void createpartition::pbCancelClicked()
{
	if( m_created == false ){
		QString s = m_ui->lineEditVolumePath->text() ;
		if(s.left(5) != QString("/dev/"))
			QFile::remove(s) ;
	}
	HideUI() ;
}

void createpartition::HideUI()
{
	this->hide();
	emit HideUISignal(this);
}

void createpartition::enableAll()
{
	m_ui->labelPassPhrase->setEnabled(true);
	m_ui->labelVolumePath->setEnabled(true);
	m_ui->labelRepeatPassPhrase->setEnabled(true);
	m_ui->lineEditPassphrase1->setEnabled(true);
	m_ui->lineEditPassPhrase2->setEnabled(true);
	m_ui->lineEditVolumePath->setEnabled(true);
	m_ui->pbCancel->setEnabled(true);
	m_ui->pbCreate->setEnabled(true);
	m_ui->pbOpenKeyFile->setEnabled(true);
	m_ui->labelfs->setEnabled(true);
	m_ui->labelvolumetype->setEnabled(true);
	m_ui->labelrng->setEnabled(true);
	m_ui->comboBoxFS->setEnabled(true);
	m_ui->comboBoxVolumeType->setEnabled(true);
	if(m_ui->comboBoxVolumeType->currentIndex() == 0)
		m_ui->comboBoxRNG->setEnabled(true);
	m_ui->rbPassphrase->setEnabled(true);
	m_ui->rbPassphraseFromFile->setEnabled(true);
}

void createpartition::disableAll()
{
	m_ui->labelPassPhrase->setEnabled(false);
	m_ui->labelVolumePath->setEnabled(false);
	m_ui->labelRepeatPassPhrase->setEnabled(false);
	m_ui->lineEditPassphrase1->setEnabled(false);
	m_ui->lineEditPassPhrase2->setEnabled(false);
	m_ui->lineEditVolumePath->setEnabled(false);
	m_ui->pbCancel->setEnabled(false);
	m_ui->pbCreate->setEnabled(false);
	m_ui->pbOpenKeyFile->setEnabled(false);
	m_ui->labelfs->setEnabled(false);
	m_ui->labelrng->setEnabled(false);
	m_ui->labelvolumetype->setEnabled(false);
	m_ui->comboBoxFS->setEnabled(false);
	m_ui->comboBoxVolumeType->setEnabled(false);
	m_ui->comboBoxRNG->setEnabled(false);
	m_ui->rbPassphrase->setEnabled(false);
	m_ui->rbPassphraseFromFile->setEnabled(false);
}

void createpartition::rbPassphraseClicked()
{
	m_ui->pbOpenKeyFile->setEnabled(false);
	m_ui->lineEditPassPhrase2->setEnabled(true);
	m_ui->lineEditPassphrase1->clear();
	m_ui->lineEditPassPhrase2->clear();
	m_ui->lineEditPassphrase1->setEchoMode(QLineEdit::Password);
	m_ui->lineEditPassPhrase2->setEchoMode(QLineEdit::Password);
	m_ui->labelPassPhrase->setText(tr("passphrase"));
	m_ui->labelRepeatPassPhrase->setEnabled(true);
}

void createpartition::rbPasssphraseFromFileClicked()
{
	m_ui->pbOpenKeyFile->setEnabled(true);
	m_ui->lineEditPassphrase1->clear();
	m_ui->lineEditPassPhrase2->clear();
	m_ui->lineEditPassphrase1->setEchoMode(QLineEdit::Normal);
	m_ui->lineEditPassPhrase2->setEnabled(false);
	m_ui->labelPassPhrase->setText(tr("key file"));
	m_ui->labelRepeatPassPhrase->setEnabled(false);
}

void createpartition::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.setFont(this->font());
	m.exec() ;
}

void createpartition::pbCreateClicked()
{
	QString volumePath   =  m_ui->lineEditVolumePath->text() ;
	QString passphrase_1 =  m_ui->lineEditPassphrase1->text() ;
	QString passphrase_2 = m_ui->lineEditPassPhrase2->text();

	if( volumePath == QString("") )	{
		UIMessage(tr("ERROR!"),tr("volume path field is empty"));
		return ;
	}
	if( volumePath.mid(0,5) == QString("UUID=")){
		if( miscfunctions::isUUIDvalid(volumePath) == false ){
			UIMessage(tr("ERROR"),tr("could not find any partition with the presented UUID"));
			return ;
		}
	}
	if( volumePath.mid(0,2) == QString("~/"))
		volumePath = QDir::homePath() + QString("/") + volumePath.mid(2) ;

	if( passphrase_1 == QString("")){
		if( m_ui->rbPassphrase->isChecked() == true)
			UIMessage(tr("ERROR"),tr("passphrases field is empty"));
		else
			UIMessage(tr("ERROR"),tr("key file field is empty"));
		return ;
	}
	if(m_ui->rbPassphrase->isChecked() == true){
		if( passphrase_1 != passphrase_2){
			UIMessage(tr("ERROR"),tr("passphrases do not match"));
			return ;
		}
	}	
	QString source ;

	if (m_ui->rbPassphraseFromFile->isChecked() == true){
		if( passphrase_1.mid(0,2) == QString("~/"))
			passphrase_1 = QDir::homePath() + QString("/") + passphrase_1.mid(2) ;
		if(miscfunctions::exists(passphrase_1) == false){
			UIMessage(tr("ERROR"),tr("invalid path to key file"));
			return ;
		}
		source = QString("-f") ;
	}else{
		source = QString("-p") ;
	}

	if( volumePath.left(5) == QString("/dev/")){
		QMessageBox m ;
		m.setFont(this->font());
		m.setParent(this);
		m.setWindowFlags(Qt::Window | Qt::Dialog);
		m.setWindowTitle(tr("WARNING"));

		m.addButton(QMessageBox::Yes);
		m.addButton(QMessageBox::No);
		m.setDefaultButton(QMessageBox::No);

		QString wr = tr("all contents of \"") ;
		wr = wr + volumePath + tr("\" will be deleted!.");
		wr = wr + tr("\nAre you sure you want to proceed?") ;
		m.setText(wr);

		if ( m.exec() != QMessageBox::Yes )
			return ;
	}

	passphrase_1.replace("\"","\"\"\"") ;
	volumePath.replace("\"","\"\"\"") ;

	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" create \"") ;
	exe = exe + volumePath + QString("\" ") ;
	exe = exe + m_ui->comboBoxFS->currentText() + QString(" ") ;
	exe = exe + m_ui->comboBoxVolumeType->currentText() + QString(" ") ;
	exe = exe +  source + QString(" \"") ;;
	exe = exe + passphrase_1 + QString("\" ") ;;
	exe = exe + m_ui->comboBoxRNG->currentText();

	m_cvt = new runInThread(exe) ;

	connect(m_cvt,
		SIGNAL(finished(runInThread *,int)),
		this,
		SLOT(threadfinished(runInThread *,int))) ;
	disableAll();
	m_cvt->start();
}

void createpartition::threadfinished(runInThread *cvt,int status)
{	
	m_created = true ;
	cvt->wait() ;
	delete cvt ;
	cvt = NULL ;
	switch( status ) {
		case 0 : UIMessage(tr("SUCCESS"),
				   tr("volume created successfully"));
			HideUI();
			break;
		case 3 : UIMessage(tr("ERROR"),
			tr("could not create an encrypted volume in a file or device"));
			break ;			
		case 6 : UIMessage(tr("ERROR"),
				   tr("couldnt get enought memory to hold the key file"));
			break ;
		case 11: UIMessage(tr("ERROR"),
			tr("can not create the volume,\"") + QString(ZULUCRYPTmkfs) + tr("\" not found."));	
		default: UIMessage(tr("ERROR"),
				   (tr("unrecognized error has occured,volume not created")));
	}
	enableAll();
}

createpartition::~createpartition()
{
    delete m_ui;
}
