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

#include "backupluksheader.h"
#include "ui_backupluksheader.h"
#include "miscfunctions.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QKeyEvent>

backupluksheader::backupluksheader(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::backupluksheader)
{
	m_ui->setupUi(this);
	this->setWindowFlags(Qt::Window | Qt::Dialog);
	this->setFont(parent->font());

	m_msg.setParent(this);

	connect(m_ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
	connect(m_ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(m_ui->pushButtonFile,SIGNAL(clicked()),this,SLOT(pbOpenFile()));
	connect(m_ui->pushButtonPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition()));

	m_backUpInProgress = false ;
}

void backupluksheader::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void backupluksheader::closeEvent(QCloseEvent *e)
{
	e->ignore();
	this->pbCancel();
}

void backupluksheader::ShowUI()
{
	m_ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));	
	m_ui->pushButtonPartition->setIcon(QIcon(QString(":/partition.png")));
	m_ui->pushButtonFile->setIcon(QIcon(QString(":/file.png")));
	m_ui->lineEditPath->setText(QDir::homePath());
	m_ui->lineEditDevicePath->setFocus();
	this->show();
}

void backupluksheader::pbCancel()
{
	if( m_backUpInProgress == false )
		this->HideUI();
}

void backupluksheader::enableAll()
{
	m_ui->label->setEnabled(true);
	m_ui->label_2->setEnabled(true);
	m_ui->label_3->setEnabled(true);
	m_ui->lineEditBackUpName->setEnabled(true);
	m_ui->lineEditDevicePath->setEnabled(true);
	m_ui->lineEditPath->setEnabled(true);
	m_ui->pbCancel->setEnabled(true);
	m_ui->pbCreate->setEnabled(true);
	m_ui->pbOpenFolder->setEnabled(true);
	m_ui->pushButtonFile->setEnabled(true);
	m_ui->pushButtonPartition->setEnabled(true);
}

void backupluksheader::disableAll()
{
	m_ui->label->setEnabled(false);
	m_ui->label_2->setEnabled(false);
	m_ui->label_3->setEnabled(false);
	m_ui->lineEditBackUpName->setEnabled(false);
	m_ui->lineEditDevicePath->setEnabled(false);
	m_ui->lineEditPath->setEnabled(false);
	m_ui->pbCancel->setEnabled(false);
	m_ui->pbCreate->setEnabled(false);
	m_ui->pbOpenFolder->setEnabled(false);
	m_ui->pushButtonFile->setEnabled(false);
	m_ui->pushButtonPartition->setEnabled(false);
}

void backupluksheader::pbCreate()
{
	QString device = miscfunctions::resolveHomeSymbol(m_ui->lineEditDevicePath->text());

	m_path = miscfunctions::resolveHomeSymbol(m_ui->lineEditPath->text()) ;

	if( device.isEmpty() )
		return m_msg.UIMessage(tr("ERROR!"),tr("path to device field is empty"));

	if( m_path.isEmpty() )
		return m_msg.UIMessage(tr("ERROR!"),tr("folder path to where the backup header will be stored is empty"));

	if(miscfunctions::exists(m_path) == false)
		return m_msg.UIMessage(tr("ERROR!"),tr("destination folder does not exist"));

	QString bkpath = m_ui->lineEditBackUpName->text();

	m_path.replace("\"","\"\"\"") ;
	bkpath.replace("\"","\"\"\"") ;
	device.replace("\"","\"\"\"") ;
	
	bkpath = m_path + QString("/") + bkpath + QString(".luksHeaderBackUp");

	QString exe = QString("%1 -B -d \"%2\" -f \"%3\"").arg(ZULUCRYPTzuluCrypt).arg(device).arg(bkpath);

	this->disableAll();

	m_backUpInProgress = true ;

	runInThread * rt = new runInThread(exe);
	connect(rt,SIGNAL(finished(int)),this,SLOT(threadExitStatus(int)));
	QThreadPool::globalInstance()->start(rt) ;

	disableAll() ;
}

void backupluksheader::pbOpenPartition()
{
	openpartition * openPartition = new openpartition(this);
	connect(openPartition,SIGNAL(clickedPartition(QString)),this,SLOT(selectedPartition(QString)));
	connect(openPartition,SIGNAL(HideUISignal()),openPartition,SLOT(deleteLater()));
	openPartition->ShowNonSystemPartitions();
}

void backupluksheader::selectedPartition(QString p)
{
	m_ui->lineEditDevicePath->setText(p);
}

void backupluksheader::pbOpenFile()
{
	QString Z = QFileDialog::getOpenFileName(this,tr("select a file volume with a luks header to backup"),QDir::homePath(),0);
	m_ui->lineEditDevicePath->setText(Z);
}

void backupluksheader::threadExitStatus(int st)
{
	m_backUpInProgress = false ;

	this->enableAll();

	switch( st ){
		case 0 : m_msg.UIMessage(tr("SUCCESS"),tr("header saved successfully" ) )						; break ;
		case 1 : m_msg.UIMessage( tr("SUCCESS"),tr("header restored successfully" ) )						; break ;
		case 2 : m_msg.UIMessage( tr("ERROR!"),tr("presented device is not a LUKS device" ) )					; break ;
		case 3 : m_msg.UIMessage( tr("ERROR!"),tr("failed to read/write header,is the volume open?" ) )				; break ;
		case 4 : m_msg.UIMessage( tr("ERROR!"),tr("failed to read/write header,is the volume open?" ) )				; break ;
		case 5 : m_msg.UIMessage( tr("INFO!"),tr("operation terminater per user request" ) )					; break ;
		case 6 : m_msg.UIMessage( tr("ERROR!"),tr("path to be used to create a back up file is occupied" )) 			; break ;
		case 7 : m_msg.UIMessage( tr("ERROR!"),tr("failed to restore" )	)							; break ;
		case 8 : m_msg.UIMessage( tr("ERROR!"),tr("insufficient privilege to open backup header file for reading" )) 		; break ;
		case 9 : m_msg.UIMessage( tr("ERROR!"),tr("invalid path to back up header file" ) )					; break ;
		case 10: m_msg.UIMessage( tr("ERROR!"),tr("insufficient privilege to create a backup file in a destination folder" )) 	; break ;
		case 11: m_msg.UIMessage( tr("ERROR!"),tr("invalid path to device" ) )							; break ;
		case 12: m_msg.UIMessage( tr("ERROR!"),tr("argument for path to a backup  header file is missing" )) 			; break ;
		case 13: m_msg.UIMessage( tr("ERROR!"),tr("argument for path to a backup  header file is missing" )) 			; break ;
		case 14: m_msg.UIMessage( tr("ERROR!"),tr("only root user can restore luks header on a system partition" )) 		; break ;
		case 15: m_msg.UIMessage( tr("ERROR!"),tr("insufficient privilege to open device for writing" ))			; break ;

	}
}

void backupluksheader::pbOpenFolder()
{
	QString p = tr("Select a folder to create a backup file in");
	QString q = QDir::homePath() ;
	QString Z = QFileDialog::getExistingDirectory(this,p,q,QFileDialog::ShowDirsOnly) ;

	m_ui->lineEditPath->setText( Z );
}


backupluksheader::~backupluksheader()
{
	miscfunctions::debug("~backupluksheader()");
	delete m_ui;
}
