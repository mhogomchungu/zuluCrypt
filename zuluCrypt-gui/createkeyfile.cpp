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
#include "../zuluCrypt-cli/executables.h"

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
	m_ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));
	connect(m_ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
	connect(m_ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	m_rng = NULL ;
}

void createkeyfile::HideUI()
{
	this->hide();
	emit HideUISignal(this);
}

void createkeyfile::closeEvent(QCloseEvent *e)
{
	e->ignore();
	HideUI();
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
	if( m_rng == NULL){
		HideUI() ;
		return ;
	}
	m_rng->terminate();  ;
	m_rng = NULL ;
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

void createkeyfile::UIMessage(QString title, QString message)
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

void createkeyfile::pbCreate()
{
	QString fileName = m_ui->lineEditFileName->text() ;
	QString path = m_ui->lineEditPath->text() ;

	m_path = m_ui->lineEditPath->text() ;

	if( fileName == QString("") ){
		UIMessage(tr("ERROR!"),("the key name field is empth"));
		return ;
	}
	if( path == QString("") ){
		UIMessage(tr("ERROR!"),("folder path to where the key will be created is empty"));
		return ;
	}
	if(m_path.mid(0,2) == QString("~/"))
		m_path = QDir::homePath() + QString("/") + m_path.mid(2) ;

	if(miscfunctions::exists(m_path) == false){
		UIMessage(tr("ERROR!"),("destination folder does not exist"));
		return ;
	}
	QString keyfile = m_path + QString("/") + m_ui->lineEditFileName->text() ;

	if( miscfunctions::exists(keyfile) == true){
		UIMessage(tr("ERROR!"),("file with the same name and at the destination folder already exist"));
		return ;
	}
	QFile o( keyfile ) ;
	o.open(QIODevice::WriteOnly) ;

	if( o.putChar('X') == false ){
		UIMessage(tr("ERROR!"),("you dont seem to have writing access to the destination folder"));
		o.close();
		return ;
	}
	o.close();
	o.remove();
	
	m_rng = new createFileThread(m_ui->comboBoxRNG->currentText(),keyfile,64,0) ;
	connect(m_rng,SIGNAL(finished()),this,SLOT(threadfinished()));
	disableAll() ;
	m_rng->start();
}

void createkeyfile::threadterminated()
{
	enableAll();
}

void createkeyfile::threadfinished()
{
	m_rng->deleteLater(); ;

	m_rng = NULL ;

	enableAll();

	QFile f(m_path + QString("/") + m_ui->lineEditFileName->text()) ;

	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setFont(this->font());

	if( f.size() == 64){
		m.setWindowTitle(tr("SUCCESS!"));
		m.setText(tr("key file successfully created"));
		m.exec() ;
	}else{
		m.setWindowTitle(tr("WARNING!"));
		m.setText(tr("process interrupted, key not fully generated"));
		m.exec() ;
	}
	this->HideUI();
}

void createkeyfile::pbOpenFolder()
{
	QString Z = QFileDialog::getExistingDirectory(this,
						      QString("Select a folder to create a key file in"),
						      QDir::homePath(),QFileDialog::ShowDirsOnly) ;

	m_ui->lineEditPath->setText( Z );
}


createkeyfile::~createkeyfile()
{
	delete m_ui;
}
