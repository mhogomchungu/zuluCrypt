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

#include "manageluksheader.h"
#include "miscfunctions.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QKeyEvent>

manageluksheader::manageluksheader(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::manageluksheader)
{
	m_ui->setupUi(this);
	this->setWindowFlags(Qt::Window | Qt::Dialog);
	this->setFont(parent->font());
	this->setFixedSize(this->size());

	m_msg.setParent(this);

	m_ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));
	m_ui->pushButtonPartition->setIcon(QIcon(QString(":/partition.png")));
	m_ui->pushButtonFile->setIcon(QIcon(QString(":/file.png")));

	m_ui->lineEditDevicePath->setFocus();

	connect(m_ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
	connect(m_ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenLuksHeaderBackUp())) ;
	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(m_ui->pushButtonFile,SIGNAL(clicked()),this,SLOT(pbOpenFile()));
	connect(m_ui->pushButtonPartition,SIGNAL(clicked()),this,SLOT(pbOpenPartition()));
	connect(m_ui->lineEditDevicePath,SIGNAL(textChanged(QString)),this,SLOT(backUpHeaderNameChange(QString)));

	m_OperationInProgress = false ;
}

void manageluksheader::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void manageluksheader::closeEvent(QCloseEvent *e)
{
	e->ignore();
	this->pbCancel();
}

void manageluksheader::ShowUI()
{
	m_ui->lineEditDevicePath->setFocus();
	this->show();
}

void manageluksheader::headerBackUp()
{
	m_operation = QString("backup") ;
	this->setWindowTitle(QString("back up luks header"));
	m_ui->labelBackUpHeader->setText(QString("backup path"));
	m_ui->pbCreate->setText(QString("&backup"));
	//m_ui->pbOpenFolder->setEnabled(false);
	//m_ui->labelBackUpHeader->setEnabled(false);
	//m_ui->lineEditBackUpName->setEnabled(false);
	this->show();
}

void manageluksheader::backUpHeader()
{
	this->headerBackUp();	
}

void manageluksheader::backUpHeader(QString device)
{
	m_ui->lineEditDevicePath->setText(device);
	this->headerBackUp();
}

void manageluksheader::backUpHeaderNameChange(QString name)
{
	if( m_operation == QString("restore"))
		return ;

	if( m_ui->lineEditDevicePath->text().isEmpty()){
		;
	}else{
		QString p = name.split("/").last() ;

		if(p.isEmpty())
			m_ui->lineEditBackUpName->clear();
		else{
			QString path = m_ui->lineEditBackUpName->text() ;

			if(path.isEmpty())
				path = QDir::homePath() + QString("/") ;

			QStringList q = path.split("/") ;
			path.clear();
			int j = q.size() - 1 ;
			for( int i = 0 ; i < j ; i++ )
				path += q.at(i) +  QString("/") ;

			path += p + QString(".luksHeaderBackUp");
			m_ui->lineEditBackUpName->setText(path);
		}
	}
}

void manageluksheader::pbOpenLuksHeaderBackUp()
{
	QString Z ;
	QString Y ;
	if(m_operation == QString("restore"))
		Z = QFileDialog::getOpenFileName(this,tr("select a file with a luks backup header"),QDir::homePath(),0);
	else{
		Z = QFileDialog::getExistingDirectory(this,tr("select a folder to store the header"),QDir::homePath(),0);

		QString p = m_ui->lineEditDevicePath->text().split("/").last() ;

		if(!p.isEmpty()){
			QString q = m_ui->lineEditBackUpName->text() ;
			if(q.isEmpty())
				Z += QString("/") + p + QString(".luksHeaderBackUp");
			else{
				q = q.split("/").last() ;
				if(q.isEmpty())
					Z += QString("/") + p + QString(".luksHeaderBackUp");
				else
					Z += QString("/") + q ;
			}
		}
	}

	m_ui->lineEditBackUpName->setText(Z);
	if(m_ui->lineEditDevicePath->text().isEmpty())
		m_ui->lineEditDevicePath->setFocus();
	else
		m_ui->pbCreate->setFocus();
}

void manageluksheader::restoreHeader()
{
	m_operation = QString("restore") ;
	this->setWindowTitle(QString("restore luks header"));
	m_ui->labelBackUpHeader->setText(QString("backup path"));
	m_ui->pbCreate->setText(QString("&restore"));
	this->show();
}

void manageluksheader::pbCancel()
{
	if( m_OperationInProgress == false )
		this->HideUI();
}

void manageluksheader::enableAll()
{
	m_ui->labelBackUpHeader->setEnabled(true);
	m_ui->labelDevicePath->setEnabled(true);
	if(m_operation == QString("restore"))
		m_ui->lineEditBackUpName->setEnabled(true);
	m_ui->lineEditDevicePath->setEnabled(true);
	m_ui->pbCancel->setEnabled(true);
	m_ui->pbCreate->setEnabled(true);
	m_ui->pbOpenFolder->setEnabled(true);
	m_ui->pushButtonFile->setEnabled(true);
	m_ui->pushButtonPartition->setEnabled(true);
}

void manageluksheader::disableAll()
{
	m_ui->labelBackUpHeader->setEnabled(false);
	m_ui->labelDevicePath->setEnabled(false);
	m_ui->lineEditBackUpName->setEnabled(false);
	m_ui->lineEditDevicePath->setEnabled(false);
	m_ui->pbCancel->setEnabled(false);
	m_ui->pbCreate->setEnabled(false);
	m_ui->pbOpenFolder->setEnabled(false);
	m_ui->pushButtonFile->setEnabled(false);
	m_ui->pushButtonPartition->setEnabled(false);
}

void manageluksheader::pbCreate()
{
	QString device = miscfunctions::resolveHomeSymbol(m_ui->lineEditDevicePath->text());

	if( device.isEmpty() )
		return m_msg.UIMessage(tr("ERROR!"),tr("path to device field is empty"));

	device.replace("\"","\"\"\"") ;
	
	QString backUp = m_ui->lineEditBackUpName->text().replace("\"","\"\"\"");

	QString exe ;
	if( m_operation == QString("backup"))
		exe = QString("%1 -B -d \"%2\" -f \"%3\"").arg(ZULUCRYPTzuluCrypt).arg(device).arg(backUp);
	else{
		UIMsg msg(this) ;
		QString x = m_ui->lineEditDevicePath->text() ;
		QString y = m_ui->lineEditBackUpName->text() ;

		QString warn = tr("Are you sure you want to replace a header on device \"%1\" with a backup copy at \"%2\"?").arg(x).arg(y);

		if(msg.UIMessageWithConfirm(tr("WARNING!"),warn) == QMessageBox::No)
			return ;
		exe = QString("%1 -kR -d \"%2\" -f \"%3\"").arg(ZULUCRYPTzuluCrypt).arg(device).arg(backUp);
	}
	
	this->disableAll();

	m_OperationInProgress = true ;

	runInThread * rt = new runInThread(exe);
	connect(rt,SIGNAL(finished(int)),this,SLOT(threadExitStatus(int)));
	rt->start();
	disableAll() ;
}

void manageluksheader::pbOpenPartition()
{
	openpartition * openPartition = new openpartition(this);
	connect(openPartition,SIGNAL(clickedPartition(QString)),this,SLOT(selectedPartition(QString)));
	connect(openPartition,SIGNAL(HideUISignal()),openPartition,SLOT(deleteLater()));
	openPartition->ShowNonSystemPartitions();
}

void manageluksheader::selectedPartition(QString p)
{
	m_ui->lineEditDevicePath->setText(p);
	if(m_ui->lineEditBackUpName->text().isEmpty())
		m_ui->lineEditBackUpName->setFocus();
	else
		m_ui->pbCreate->setFocus();
}

void manageluksheader::pbOpenFile()
{
	QString Z = QFileDialog::getOpenFileName(this,tr("select luks container you want to backup its header"),QDir::homePath(),0);
	m_ui->lineEditDevicePath->setText(Z);
	if(m_ui->lineEditBackUpName->text().isEmpty())
		m_ui->lineEditBackUpName->setFocus();
	else
		m_ui->pbCreate->setFocus();}

void manageluksheader::threadExitStatus(int st)
{
	m_OperationInProgress = false ;

	switch( st ){
		case 0 : m_msg.UIMessage( tr("SUCCESS"),tr("header saved successfully" ) ) ;
			 return this->HideUI();
		case 1 : m_msg.UIMessage( tr("SUCCESS"),tr("header restored successfully" ) )	;
			 return this->HideUI();
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
		case 14: m_msg.UIMessage( tr("ERROR!"),tr("only root user can restore luks header on a system device" )) 		; break ;
		case 15: m_msg.UIMessage( tr("ERROR!"),tr("insufficient privilege to open device for writing" ))			; break ;
		case 16: m_msg.UIMessage( tr("ERROR!"),tr("could not resolve path to device" ))						; break ;
		case 17: m_msg.UIMessage( tr("ERROR!"),tr("backup file does not appear to contain luks header" ))			; break ;
	}
	this->enableAll();
}

manageluksheader::~manageluksheader()
{
	delete m_ui;
}
