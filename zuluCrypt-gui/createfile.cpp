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

#include "ui_createfile.h"
#include "createfile.h"
#include "miscfunctions.h"
#include "../zuluCrypt-cli/executables.h"

#include <QFileDialog>
#include <QFile>

#include <iostream>

createfile::createfile(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::createfile)
{
	m_ui->setupUi(this);
	this->setFixedSize(this->size());

	m_mb.addButton(QMessageBox::Yes);
	m_mb.addButton(QMessageBox::No);
	m_mb.setDefaultButton(QMessageBox::No);

	m_ui->progressBar->setMinimum(0);
	m_ui->progressBar->setMaximum(100);
	m_ui->progressBar->setValue(0);

	m_ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));

	m_time.setInterval(250);

	connect(static_cast<QObject *>(&m_time),SIGNAL(timeout()),this,SLOT(monitorFileGrowth()));
	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(m_ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(m_ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
}

void createfile::closeEvent(QCloseEvent *e)
{
	e->ignore();
	pbCancel() ;
}

void createfile::enableAll()
{
	m_ui->lineEditFileName->setEnabled(true);
	m_ui->lineEditFilePath->setEnabled(true);
	m_ui->lineEditFileSize->setEnabled(true);
	m_ui->comboBox->setEnabled(true);
	m_ui->comboBoxRNG->setEnabled(true);
	m_ui->pbOpenFolder->setEnabled(true);
	m_ui->label->setEnabled(true);
	m_ui->label_2->setEnabled(true);
	m_ui->label_3->setEnabled(true);
	m_ui->label_4->setEnabled(true);
	m_ui->label_5->setEnabled(true);
	m_ui->pbCreate->setEnabled(true);
}

void createfile::disableAll()
{
	m_ui->pbCreate->setEnabled(false);
	m_ui->lineEditFileName->setEnabled(false);
	m_ui->lineEditFilePath->setEnabled(false);
	m_ui->lineEditFileSize->setEnabled(false);
	m_ui->comboBox->setEnabled(false);
	m_ui->comboBoxRNG->setEnabled(false);
	m_ui->pbOpenFolder->setEnabled(false);
	m_ui->label->setEnabled(false);
	m_ui->label_2->setEnabled(false);
	m_ui->label_3->setEnabled(false);
	m_ui->label_4->setEnabled(false);
	m_ui->label_5->setEnabled(false);
}

void createfile::showUI()
{
	enableAll();
	m_ui->comboBox->setCurrentIndex(1) ;
	m_ui->comboBoxRNG->setCurrentIndex(0);
	m_ui->lineEditFileName->clear();
	m_ui->lineEditFilePath->setText(QDir::homePath());
	m_ui->lineEditFileSize->clear();
	m_ui->progressBar->setValue(0);
	m_ui->lineEditFileName->setFocus();
	m_creating = false ;
	this->show();
}

void createfile::UIMessage(QString title, QString message)
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

void createfile::pbCreate()
{
	QString fileName = m_ui->lineEditFileName->text() ;
	QString filePath = m_ui->lineEditFilePath->text() ;
	QString fileSize = m_ui->lineEditFileSize->text() ;

	m_path = m_ui->lineEditFilePath->text() ;

	if(fileName == QString("")){
		UIMessage(tr("ERROR!"),("file name field is empty"));
		return ;
	}
	if(filePath == QString("")){
		UIMessage(tr("ERROR!"),("file path field is empty"));
		return ;
	}
	if(fileSize == QString("")){
		UIMessage(tr("ERROR!"),("file size field is empty"));
		return ;
	}	
	if ( m_path.mid(0,2) == QString("~/"))
		m_path = QDir::homePath() + QString("/") + m_path.mid(2) ;

	if(miscfunctions::exists(m_path) == false ){
		UIMessage(tr("ERROR!"),("destination folder does not exist"));
		return ;
	}
	if(miscfunctions::exists(m_path + QString("/") + fileName)){
		UIMessage(tr("ERROR!"),("a file or folder with the same name already exist at destination address"));
		return ;
	}
	bool test ;

	fileSize.toInt(&test) ;

	if( test == false ){
		UIMessage(tr("ERROR!"),("Illegal character in the file size field.Only digits are allowed"));
		return ;
	}
	QString source = m_ui->comboBoxRNG->currentText() ;

	m_path = m_path + QString("/") + fileName;

	if( miscfunctions::exists(m_path) == true){
		UIMessage(tr("ERROR!"),("file with the same name and at the destination folder already exist"));
		return ;
	}
	QFile f(m_path);
	f.open(QIODevice::WriteOnly) ;

	if( f.putChar('X') == false ){
		f.close();
		UIMessage(tr("ERROR!"),("you dont seem to have writing access to the destination folder"));
		return ;
	}
	f.close();
	f.remove() ;
	switch( m_ui ->comboBox->currentIndex()){
		case 0 :m_fileSize = fileSize.toDouble() * 1024 ;
			break ;
		case 1 :m_fileSize = fileSize.toDouble() * 1024 * 1024 ;
			break ;
		case 2 :m_fileSize = fileSize.toDouble() * 1024 * 1024  * 1024;
			break ;
	}
	if( m_fileSize < 3145728 ){
		UIMessage(tr("ERROR!"),("container file must be bigger than 3MB"));
		return ;
	}
	disableAll();
	m_time.start();
	m_creating = true ;
	m_terminated = false ;
	
	m_cft = new createFileThread( source, m_path,m_fileSize,1 ) ;

	connect(m_cft,SIGNAL(finished()),
		this,SLOT(createFileThreadFinished()));
	m_cft->start();
}

void createfile::createFileThreadFinished()
{
	m_time.stop();
	m_cft->deleteLater(); ;

	if( m_mb.isVisible() == true ){
		m_mb.hide();
		m_return = true ;
	}
	if( m_terminated == true )
		return ;

	emit fileCreated( m_path ) ;
	HideUI();
}

void createfile::HideUI()
{
	this->hide();
	emit HideUISignal(this);
}

void createfile::monitorFileGrowth()
{
	QFileInfo f( m_path ) ;
	m_ui->progressBar->setValue(f.size() * 100 / m_fileSize);
}

void createfile::pbOpenFolder()
{
	QString Z = QFileDialog::getExistingDirectory(this,
						      tr("Select Path to where the file will be created"),
						      QDir::homePath(),QFileDialog::ShowDirsOnly) ;

	m_ui->lineEditFilePath->setText( Z );
}

void createfile::pbCancel()
{
	if( m_creating == false ){
		HideUI();
		return ;
	}
	m_mb.setWindowTitle(tr("terminating file creation process"));
	m_mb.setParent(this);
	m_mb.setWindowFlags(Qt::Window | Qt::Dialog);
	m_mb.setText(tr("are you sure you want to stop file creation process?"));
	m_mb.setFont(this->font());

	m_return = false ;

	if(m_mb.exec() == QMessageBox::No)
		return ;
	/*
	  Below check is necessary because a user may cancel a file
	  creation operation but fail to confirm in time. This will
	  lead to a situation where one line of execution will be here and another
	  will be in "createFileThreadFinished()" function above.

	  The value set in that function will have an end result of autoselecting "no"
	  to the user.
	  */
	if( m_return == true )
		return ;

	m_cft->terminate();
	m_terminated = true ;
	m_creating = false ;
	m_time.stop();
	QFile::remove( m_path ) ;
	HideUI();
}

createfile::~createfile()
{
	delete m_ui;
}
