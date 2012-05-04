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
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QFile>

#include <iostream>

createfile::createfile(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::createfile)
{
	m_ui->setupUi(this);
	this->setFixedSize(this->size());
	this->setWindowFlags(Qt::Window | Qt::Dialog);
	this->setFont(parent->font());

	m_mb.setWindowTitle(tr("terminating file creation process"));
	m_mb.setParent(this);
	m_mb.setWindowFlags(Qt::Window | Qt::Dialog);
	m_mb.setText(tr("are you sure you want to stop file creation process?"));
	m_mb.setFont(this->font());

	m_msg.setParent(this);

	m_yes = m_mb.addButton(tr("dummy"),QMessageBox::YesRole);
	m_no  = m_mb.addButton(tr("dummy1"),QMessageBox::NoRole);

	m_ui->progressBar->setMinimum(0);
	m_ui->progressBar->setMaximum(100);
	m_ui->progressBar->setValue(0);

	m_ui->pbOpenFolder->setIcon(QIcon(QString(":/folder.png")));

	m_cft = NULL ;

	connect(m_ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;
	connect(m_ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;
	connect(m_ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;
}

void createfile::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if(m_cft != NULL)
		return ;
	pbCancel() ;
}

void createfile::enableAll()
{
	m_ui->lineEditFileName->setEnabled(true);
	m_ui->lineEditFilePath->setEnabled(true);
	m_ui->lineEditFileSize->setEnabled(true);
	m_ui->pbOpenFolder->setEnabled(true);
	m_ui->label->setEnabled(true);
	m_ui->label_2->setEnabled(true);
	m_ui->label_3->setEnabled(true);
	m_ui->label_4->setEnabled(true);
	m_ui->pbCreate->setEnabled(true);
}

void createfile::disableAll()
{
	m_ui->pbCreate->setEnabled(false);
	m_ui->lineEditFileName->setEnabled(false);
	m_ui->lineEditFilePath->setEnabled(false);
	m_ui->lineEditFileSize->setEnabled(false);
	m_ui->comboBox->setEnabled(false);
	m_ui->pbOpenFolder->setEnabled(false);
	m_ui->label->setEnabled(false);
	m_ui->label_2->setEnabled(false);
	m_ui->label_3->setEnabled(false);
	m_ui->label_4->setEnabled(false);
}

void createfile::showUI()
{
	enableAll();
	m_ui->comboBox->setCurrentIndex(1) ;
	m_ui->lineEditFileName->clear();
	m_ui->lineEditFilePath->setText(QDir::homePath());
	m_ui->lineEditFileSize->clear();
	m_ui->progressBar->setValue(0);
	m_ui->lineEditFileName->setFocus();
	this->setWindowTitle(tr("1/2 create container file"));
	this->show();
}

void createfile::pbCreate()
{
	QString fileName = m_ui->lineEditFileName->text() ;
	QString filePath = m_ui->lineEditFilePath->text() ;
	QString fileSize = m_ui->lineEditFileSize->text() ;

	m_path = miscfunctions::resolveHomeSymbol(m_ui->lineEditFilePath->text()) ;

	if(fileName.isEmpty())
		return m_msg.UIMessage(tr("ERROR!"),tr("file name field is empty"));

	if(filePath.isEmpty())
		return m_msg.UIMessage(tr("ERROR!"),tr("file path field is empty"));

	if(fileSize.isEmpty())
		return m_msg.UIMessage(tr("ERROR!"),tr("file size field is empty"));

	if(miscfunctions::exists(m_path) == false )
		return m_msg.UIMessage(tr("ERROR!"),tr("destination folder does not exist"));

	if(miscfunctions::exists(m_path + QString("/") + fileName))
		return m_msg.UIMessage(tr("ERROR!"),tr("a file or folder with the same name already exist at destination address"));

	bool test ;

	fileSize.toInt(&test) ;

	if( test == false )
		return m_msg.UIMessage(tr("ERROR!"),tr("Illegal character in the file size field.Only digits are allowed"));

	m_path = m_path + QString("/") + fileName;

	if( miscfunctions::exists(m_path) == true )
		return m_msg.UIMessage(tr("ERROR!"),tr("file with the same name and at the destination folder already exist"));

	if( miscfunctions::canCreateFile(m_path) == false ){
		m_msg.UIMessage(tr("ERROR!"),tr("you dont seem to have writing access to the destination folder"));
		m_ui->lineEditFilePath->setFocus();
		return ;
	}
	switch( m_ui ->comboBox->currentIndex() ){
		case 0 :m_fileSize = fileSize.toDouble() * 1024 ;
			break ;
		case 1 :m_fileSize = fileSize.toDouble() * 1024 * 1024 ;
			break ;
		case 2 :m_fileSize = fileSize.toDouble() * 1024 * 1024  * 1024;
			break ;
	}
	if( m_fileSize < 3145728 )
		return m_msg.UIMessage(tr("ERROR!"),tr("container file must be bigger than 3MB"));

	disableAll();
	
	m_cft = new createFileThread( m_path,m_fileSize ) ;

	connect(m_cft,SIGNAL(doneCreatingFile()),this,SLOT(doneCreatingFile()));
	connect(m_cft,SIGNAL(progress(int)),this,SLOT(progress(int)));
	connect(this,SIGNAL(cancelOperation()),m_cft,SLOT(cancelOperation()));

	/*
	  exitStatus will be 1 if the thread is terminated
	  exitStatus will be 0 if the thread is left to finish its work
	  */
	connect(m_cft,SIGNAL(exitStatus(int)),this,SLOT(exitStatus(int))) ;

	QThreadPool::globalInstance()->start(m_cft);
}

void createfile::exitStatus(int status)
{
	m_cft = NULL ;

	if( status == 1 )
		QFile::remove( m_path ) ;
	else{
		if( m_mb.isVisible() == true )
			m_mb.hide();

		emit fileCreated( m_path ) ;
	}

	HideUI();
}

void createfile::pbCancel()
{
	if(m_cft == NULL)
		return HideUI();

	m_mb.removeButton(m_yes);
	m_mb.removeButton(m_no);

	m_yes = m_mb.addButton(tr("yes"),QMessageBox::YesRole);
	m_no  = m_mb.addButton(tr("no"),QMessageBox::NoRole);

	m_mb.exec() ;

	if(m_mb.clickedButton() == m_yes)
		emit cancelOperation();
}

void createfile::HideUI()
{
	emit HideUISignal();
	this->hide();	
}

void createfile::progress(int p)
{
	m_ui->progressBar->setValue(p);
}

void createfile::doneCreatingFile()
{
	m_ui->progressBar->setValue(0);
	this->setWindowTitle(tr("2/2 write random data to container file"));

}

void createfile::pbOpenFolder()
{
	QString p = tr("Select Path to where the file will be created") ;
	QString q = QDir::homePath();
	QString Z = QFileDialog::getExistingDirectory(this,p,q,QFileDialog::ShowDirsOnly) ;

	m_ui->lineEditFilePath->setText( Z );
}

createfile::~createfile()
{
	delete m_ui;
}
