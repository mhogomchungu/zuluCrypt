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

#include "createkeyfile.h"
#include "ui_createkeyfile.h"
#include "miscfunctions.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QKeyEvent>

createkeyfile::createkeyfile(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::createkeyfile)
{
	m_ui->setupUi(this);
	this->setWindowFlags(Qt::Window | Qt::Dialog);
	this->setFont(parent->font());

	m_ckt = NULL ;

	m_ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));
	connect(m_ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
	connect(m_ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
}

void createkeyfile::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void createkeyfile::closeEvent(QCloseEvent *e)
{
	e->ignore();
	this->pbCancel();
}

void createkeyfile::ShowUI()
{
	m_ui->lineEditFileName->clear();
	m_ui->lineEditPath->setText(QDir::homePath());
	m_ui->comboBoxRNG->setCurrentIndex(0);
	this->show();
}

void createkeyfile::pbCancel()
{
	if( m_ckt == NULL )
		HideUI();
	else
		m_ckt->cancelOperation();
}

void createkeyfile::enableAll()
{
	m_ui->label->setEnabled(true);
	m_ui->label_2->setEnabled(true);
	m_ui->lineEditFileName->setEnabled(true);
	m_ui->lineEditPath->setEnabled(true);
	m_ui->pbCreate->setEnabled(true);
	m_ui->pbOpenFolder->setEnabled(true);
	m_ui->labelRNG->setEnabled(true);
	m_ui->comboBoxRNG->setEnabled(true);
}

void createkeyfile::disableAll()
{
	m_ui->label->setEnabled(false);
	m_ui->label_2->setEnabled(false);
	m_ui->lineEditFileName->setEnabled(false);
	m_ui->lineEditPath->setEnabled(false);
	m_ui->pbCreate->setEnabled(false);
	m_ui->pbOpenFolder->setEnabled(false);
	m_ui->labelRNG->setEnabled(false);
	m_ui->comboBoxRNG->setEnabled(false);
}

void createkeyfile::pbCreate()
{
	DialogMsg msg(this);

	QString fileName = m_ui->lineEditFileName->text() ;
	m_path = miscfunctions::resolveHomeSymbol(m_ui->lineEditPath->text()) ;

	if( fileName.isEmpty() )
		return msg.ShowUIOK(tr("ERROR!"),tr("the key name field is empth"));

	if( m_path.isEmpty() )
		return msg.ShowUIOK(tr("ERROR!"),tr("folder path to where the key will be created is empty"));

	if(miscfunctions::exists(m_path) == false)
		return msg.ShowUIOK(tr("ERROR!"),tr("destination folder does not exist"));

	QString keyfile = m_path + QString("/") + m_ui->lineEditFileName->text() ;

	if( miscfunctions::exists(keyfile) == true)
		return msg.ShowUIOK(tr("ERROR!"),tr("file with the same name and at the destination folder already exist"));

	if( miscfunctions::canCreateFile(keyfile) == false ){
		msg.ShowUIOK(tr("ERROR!"),tr("you dont seem to have writing access to the destination folder"));
		m_ui->lineEditPath->setFocus();
		return ;
	}

	disableAll() ;

	m_ckt = new createkeyfilethread(keyfile,m_ui->comboBoxRNG->currentText());
	connect(m_ckt,SIGNAL(exitStatus(int)),this,SLOT(threadExitStatus(int)));
	m_ckt->start();
}

void createkeyfile::threadExitStatus(int st)
{
	DialogMsg msg(this) ;

	m_ckt = NULL ;
	if( st == 1 ){
		msg.ShowUIOK(tr("WARNING!"),tr("process interrupted,key not fully generated"));
		this->enableAll();
	}else{
		msg.ShowUIOK(tr("SUCCESS!"),tr("key file successfully created"));
		this->HideUI();
	}
}

void createkeyfile::pbOpenFolder()
{
	QString p = tr("Select a folder to create a key file in");
	QString q = QDir::homePath() ;
	QString Z = QFileDialog::getExistingDirectory(this,p,q,QFileDialog::ShowDirsOnly) ;

	m_ui->lineEditPath->setText( Z );
}


createkeyfile::~createkeyfile()
{
	delete m_ui;
}
