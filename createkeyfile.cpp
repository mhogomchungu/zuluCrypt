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



#include "createkeyfile.h"
#include "ui_createkeyfile.h"
#include "executables.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>

createkeyfile::createkeyfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createkeyfile)
{
	ui->setupUi(this);

	connect(ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
	connect(ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;

	rng = NULL ;
}

void createkeyfile::HideUI()
{
	this->hide();
}

void createkeyfile::ShowUI()
{
	ui->lineEditFileName->clear();
	ui->lineEditPath->setText(QDir::homePath());
	this->show();
}

void createkeyfile::pbCancel()
{
	if( rng == NULL){
		HideUI() ;
		return ;
	}

	rng->terminate();  ;
	rng = NULL ;
}

void createkeyfile::enableAll()
{
	ui->label->setEnabled(true);
	ui->label_2->setEnabled(true);
	ui->lineEditFileName->setEnabled(true);
	ui->lineEditPath->setEnabled(true);
	ui->pbCreate->setEnabled(true);
	ui->pbOpenFolder->setEnabled(true);
}

void createkeyfile::disableAll()
{
	ui->label->setEnabled(false);
	ui->label_2->setEnabled(false);
	ui->lineEditFileName->setEnabled(false);
	ui->lineEditPath->setEnabled(false);
	ui->pbCreate->setEnabled(false);
	ui->pbOpenFolder->setEnabled(false);
}

void createkeyfile::pbCreate()
{
	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setFont(this->font());

	if( ui->lineEditFileName->text().isEmpty() == true ){

		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("the key name field is empth"));
		m.exec() ;
		return ;
	}
	if( ui->lineEditPath->text().isEmpty() == true ){

		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("folder path to where the key will be created is empty"));
		m.exec() ;
		return ;
	}

	QString keyfile = ui->lineEditPath->text() + QString("/") + ui->lineEditFileName->text() ;

	QFile o( keyfile ) ;

	if( o.exists() == true){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("file with the same name and at the destination folder already exist"));
		m.exec() ;
		return ;
	}

	o.open(QIODevice::WriteOnly) ;

	if( o.putChar('X') == false ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("you dont seem to have writing access to the destination folder"));
		m.exec() ;
		return ;
	}

	o.close();

	QFile f(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray b = f.readAll() ;

	f.close();

	rng = new rngThread(QString( b.data() ),keyfile) ;

	connect(rng,SIGNAL(finished()),this,SLOT(threadfinished()));

	//connect(rng,SIGNAL(terminated()),this,SLOT(threadterminated())) ;

	disableAll() ;

	rng->start();
}

void createkeyfile::threadterminated()
{
	enableAll();
}

void createkeyfile::threadfinished()
{
	delete rng ;

	rng = NULL ;

	enableAll();

	QFile f(ui->lineEditPath->text() + QString("/") + ui->lineEditFileName->text()) ;

	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setFont(this->font());

	if( f.size() == 32){
		m.setWindowTitle(QString("SUCCESS!"));
		m.setText(QString("key file successfully created"));
		m.exec() ;
	}else{
		m.setWindowTitle(QString("WARNING!"));
		m.setText(QString("process interrupted, key not fully generated"));
		m.exec() ;
	}
	this->HideUI();
}

void createkeyfile::pbOpenFolder()
{
	QString Z = QFileDialog::getExistingDirectory((QWidget *) this,QString("Select a folder to create a key file in"),QDir::homePath(),QFileDialog::ShowDirsOnly) ;

	ui->lineEditPath->setText( Z );
}


createkeyfile::~createkeyfile()
{
	delete ui;
}
