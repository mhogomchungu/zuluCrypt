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
	this->HideUI();
}

void createkeyfile::pbCreate()
{
	QMessageBox m ;
	m.addButton(QMessageBox::Ok);
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);

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

	QFile out( keyfile) ;

	if( out.exists() == true){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("file with the same name and at the destination folder already exist"));
		m.exec() ;
		return ;
	}

	out.open(QIODevice::WriteOnly) ;

	if( out.putChar('X') == false ){
		m.setWindowTitle(QString("ERROR!"));
		m.setText(QString("you dont seem to have writing access to the destination folder"));
		m.exec() ;
		return ;
	}

	out.seek(0) ;

	QFile in(QString("/dev/urandom")) ;

	in.open(QIODevice::ReadOnly) ;

	char data ;

	for( int i = 0 ; i < 512 ; i++){

		do{
			in.getChar(&data) ;
		}while( data < 32 || data > 126) ;

		out.putChar(data) ;
	}

	in.close();
	out.close();

	m.setWindowTitle(QString("SUCCESS!"));
	m.setText(QString("key file successfully created"));
	m.exec() ;

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
