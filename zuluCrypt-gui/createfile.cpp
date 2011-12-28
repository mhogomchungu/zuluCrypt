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

#include "createfile.h"
#include "ui_createfile.h"
#include "../zuluCrypt-cli/executables.h"

#include <QFileDialog>
#include <QFile>

#include <iostream>

createfile::createfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createfile)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());

	mb.addButton(QMessageBox::Yes);
	mb.addButton(QMessageBox::No);
	mb.setDefaultButton(QMessageBox::No);

	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(100);
	ui->progressBar->setValue(0);

	ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));

	time.setInterval(250);

	connect((QObject *)&time,SIGNAL(timeout()),this,SLOT(monitorFileGrowth()));
	connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
}

void createfile::closeEvent(QCloseEvent *e)
{
	e->ignore();
	pbCancel() ;
}

void createfile::enableAll()
{
	ui->lineEditFileName->setEnabled(true);
	ui->lineEditFilePath->setEnabled(true);
	ui->lineEditFileSize->setEnabled(true);
	ui->comboBox->setEnabled(true);
	ui->comboBoxRNG->setEnabled(true);
	ui->pbOpenFolder->setEnabled(true);
	ui->label->setEnabled(true);
	ui->label_2->setEnabled(true);
	ui->label_3->setEnabled(true);
	ui->label_4->setEnabled(true);
	ui->label_5->setEnabled(true);
	ui->pbCreate->setEnabled(true);
}

void createfile::disableAll()
{
	ui->pbCreate->setEnabled(false);
	ui->lineEditFileName->setEnabled(false);
	ui->lineEditFilePath->setEnabled(false);
	ui->lineEditFileSize->setEnabled(false);
	ui->comboBox->setEnabled(false);
	ui->comboBoxRNG->setEnabled(false);
	ui->pbOpenFolder->setEnabled(false);
	ui->label->setEnabled(false);
	ui->label_2->setEnabled(false);
	ui->label_3->setEnabled(false);
	ui->label_4->setEnabled(false);
	ui->label_5->setEnabled(false);
}

void createfile::showUI()
{
	enableAll();
	ui->comboBox->setCurrentIndex(1) ;
	ui->comboBoxRNG->setCurrentIndex(0);
	ui->lineEditFileName->clear();
	ui->lineEditFilePath->setText(QDir::homePath());
	ui->lineEditFileSize->clear();
	ui->progressBar->setValue(0);
	ui->lineEditFileName->setFocus();
	creating = false ;
	this->show();
}

void createfile::pbCreate()
{
	QMessageBox m ;
	m.setWindowTitle(tr("ERROR!"));
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setFont(this->font());

	if(ui->lineEditFileName->text().isEmpty()){
		m.setText(tr("file name field is empty"));
		m.exec() ;
		return ;
	}
	if(ui->lineEditFilePath->text().isEmpty()){
		m.setText(tr("file path field is empty"));
		m.exec() ;
		return ;
	}
	if(ui->lineEditFileSize->text().isEmpty()){
		m.setText(tr("file size field is empty"));
		m.exec() ;
		return ;
	}
	path = ui->lineEditFilePath->text() ;

	if ( path.mid(0,2) == QString("~/"))
		path = QDir::homePath() + QString("/") + path.mid(2) ;

	QDir dir(path) ;
	if(dir.exists() == false ){
		m.setText(tr("destination folder does not exist"));
		m.exec() ;
		return ;
	}
	if(QFile::exists(path + QString("/") + ui->lineEditFileName->text())){
		m.setText(tr("a file or folder with the same name already exist at destination address"));
		m.exec() ;
		return ;
	}
	bool test ;

	ui->lineEditFileSize->text().toInt(&test) ;

	if( test == false ){
		m.setText(tr("Illegal character in the file size field.Only digits are allowed"));
		m.exec() ;
		return ;
	}
	QString source = ui->comboBoxRNG->currentText() ;

	path = path + QString("/") +ui->lineEditFileName->text();

	QFile f( path ) ;

	if( f.exists() == true){
		m.setText(tr("file with the same name and at the destination folder already exist"));
		m.exec() ;
		return ;
	}
	f.open(QIODevice::WriteOnly) ;

	if( f.putChar('X') == false ){
		f.close();
		m.setText(tr("you dont seem to have writing access to the destination folder"));
		m.exec() ;
		return ;
	}
	f.close();
	f.remove() ;

	QString size ;

	switch( ui ->comboBox->currentIndex()){
		case 0 :fileSize = ui->lineEditFileSize->text().toDouble() * 1024 ;
			break ;
		case 1 :fileSize = ui->lineEditFileSize->text().toDouble() * 1024 * 1024 ;
			break ;
		case 2 :fileSize = ui->lineEditFileSize->text().toDouble() * 1024 * 1024  * 1024;
			break ;
	}
	if( fileSize < 3145728 ){
		m.setText(tr("container file must be bigger than 3MB"));
		m.exec() ;
		return ;
	}
	disableAll();
	time.start();
	creating = true ;
	terminated = false ;
	
	cft = new createFileThread( source, path,fileSize,1 ) ;

	connect(cft,SIGNAL(finished()),
		this,SLOT(createFileThreadFinished()));
	cft->start();
}

void createfile::createFileThreadFinished()
{
	time.stop();
	delete cft ;

	if( mb.isVisible() == true ){
		mb.hide();
		Return = true ;
	}
	if( terminated == true )
		return ;

	emit fileCreated( path ) ;
	HideUI();
}

void createfile::HideUI()
{
	this->hide();
	emit HideUISignal(this);
}

void createfile::monitorFileGrowth()
{
	QFileInfo f( path ) ;
	ui->progressBar->setValue(f.size() * 100 / fileSize);
}

void createfile::pbOpenFolder()
{
	QString Z = QFileDialog::getExistingDirectory(this,
						      tr("Select Path to where the file will be created"),
						      QDir::homePath(),QFileDialog::ShowDirsOnly) ;

	ui->lineEditFilePath->setText( Z );
}

void createfile::pbCancel()
{
	if( creating == false ){
		this->hide();
		return ;
	}
	mb.setWindowTitle(tr("terminating file creation process"));
	mb.setParent(this);
	mb.setWindowFlags(Qt::Window | Qt::Dialog);
	mb.setText(tr("are you sure you want to stop file creation process?"));
	mb.setFont(this->font());

	Return = false ;

	if(mb.exec() == QMessageBox::No)
		return ;
	/*
	  Below check is necessary because a user may cancel a file
	  creation operation but fail to confirm in time. This will
	  lead to a situation where one line of execution will be here and another
	  will be in "createFileThreadFinished()" function above.

	  The value set in that function will have an end result of autoselecting "no"
	  to the user.
	  */
	if( Return == true )
		return ;

	cft->terminate();
	terminated = true ;
	creating = false ;
	time.stop();
	QFile::remove( path ) ;
	HideUI();
}

createfile::~createfile()
{
	delete ui;
}
