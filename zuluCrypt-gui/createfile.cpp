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

#include "createfile.h"
#include "ui_createfile.h"
#include "executables.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <iostream>

createfile::createfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createfile)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());

	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(100);
	ui->progressBar->setValue(0);

	time.setInterval(250);

	connect((QObject *)&time,SIGNAL(timeout()),this,SLOT(monitorFileGrowth()));

	connect(ui->pbCancel,SIGNAL(clicked()),this,SLOT(pbCancel())) ;

	connect(ui->pbOpenFolder,SIGNAL(clicked()),this,SLOT(pbOpenFolder())) ;

	connect(ui->pbCreate,SIGNAL(clicked()),this,SLOT(pbCreate())) ;

	connect((QObject *)&dd,SIGNAL(finished(int,QProcess::ExitStatus)),
		this,SLOT(ddFinished(int,QProcess::ExitStatus)));
}

void createfile::closeEvent(QCloseEvent *e)
{
	e->ignore();
	pbCancel() ;
}

void createfile::ddFinished(int exitCode, QProcess::ExitStatus st)
{
	QMessageBox m ;
	m.setWindowTitle(tr("ERROR!"));
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Ok);
	m.setFont(this->font());

	if( st == QProcess::CrashExit){
	//	m.setText(QString("A problem occured and creation process is terminated.
	//	\nMake sure file path field points to a folder that exists and you have writing access"));
	//	m.exec() ;
		return ;
	}
	if( exitCode != 0 ){
		m.setText(tr("you dont seem to have writing access to the destination folder"));
		m.exec() ;
		ui->pbCreate->setEnabled(true);
		ui->lineEditFileName->setEnabled(true);
		ui->lineEditFilePath->setEnabled(true);
		ui->lineEditFileSize->setEnabled(true);
		return ;
	}
	time.stop();
	this->hide();
	emit fileCreated(ui->lineEditFilePath->text() + "/" + ui->lineEditFileName->text()) ;
}

void createfile::showUI()
{
	ui->comboBox->setCurrentIndex(0) ;
	ui->lineEditFileName->clear();
	ui->lineEditFilePath->setText(QDir::homePath());
	ui->lineEditFileSize->clear();
	ui->pbCreate->setEnabled(true);
	ui->lineEditFileName->setEnabled(true);
	ui->lineEditFilePath->setEnabled(true);
	ui->lineEditFileSize->setEnabled(true);
	ui->progressBar->setValue(0);
	ui->comboBox->setEnabled(true);
	ui->pbOpenFolder->setEnabled(true);
	ui->label->setEnabled(true);
	ui->label_2->setEnabled(true);
	ui->label_3->setEnabled(true);
	ui->label_4->setEnabled(true);
	ui->lineEditFileName->setFocus();
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
		m.setText(tr("ERROR: the file name field is empty"));
		m.exec() ;
		return ;
	}

	if(ui->lineEditFilePath->text().isEmpty()){
		m.setText(tr("ERROR: the file path field is empty"));
		m.exec() ;
		return ;
	}

	if(ui->lineEditFileSize->text().isEmpty()){
		m.setText(tr("ERROR: the file size field is empty"));
		m.exec() ;
		return ;
	}

	bool test ;

	ui->lineEditFileSize->text().toInt(&test) ;

	if( test == false ){
		m.setText(tr("ERROR:Illegal character in the file size field.\nOnly digits are allowed"));
		m.exec() ;
		return ;
	}

	ui->pbCreate->setEnabled(false);
	ui->lineEditFileName->setEnabled(false);
	ui->lineEditFilePath->setEnabled(false);
	ui->lineEditFileSize->setEnabled(false);
	ui->comboBox->setEnabled(false);
	ui->pbOpenFolder->setEnabled(false);
	ui->label->setEnabled(false);
	ui->label_2->setEnabled(false);
	ui->label_3->setEnabled(false);
	ui->label_4->setEnabled(false);

	time.start();

	QString size ;

	switch( ui ->comboBox->currentIndex()){
		case 0 : { size = ""   ;  fileSize = ui->lineEditFileSize->text().toDouble() * 1024 ; }
			break ;
		case 1 : { size = "K"  ;  fileSize = ui->lineEditFileSize->text().toDouble() * 1024 * 1024 ; }
			break ;
		case 2 : { size = "M"  ;  fileSize = ui->lineEditFileSize->text().toDouble() * 1024 * 1024  * 1024 ; }
			break ;
	}

	QFile f(QDir::homePath() + QString("/.zuluCrypt/rng")) ;

	f.open(QIODevice::ReadOnly) ;

	QByteArray b = f.readAll() ;

	f.close();

	QString ddExe = QString(ZULUCRYPTdd) ;
	ddExe = ddExe + QString(" if=") ;
	ddExe = ddExe + QString( b ) ;
	ddExe = ddExe + QString(" of=") ;
	ddExe = ddExe + QString("\"") + ui->lineEditFilePath->text() + QString("/") + ui->lineEditFileName->text();
	ddExe = ddExe + QString("\" bs=1024 count=") + ui->lineEditFileSize->text() + size;

	dd.start( ddExe ) ;
}

void createfile::monitorFileGrowth()
{
	QFileInfo f(ui->lineEditFilePath->text() + "/" + ui->lineEditFileName->text()) ;

	/*
	  the QProgressBar uses signed interger for max, min and setValue.
	  This means it can not work with files greater than 2GB.

	  Rescalling min and max and current values to within 0 and 100
	  */
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
	QMessageBox m ;
	m.setWindowTitle(tr("terminating file creation process"));
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	m.setDefaultButton(QMessageBox::No);
	m.setText(tr("sure you want to stop file creation process?"));
	m.setFont(this->font());

	if(m.exec() == QMessageBox::No)
		return ;

	dd.close();

	time.stop();

	QFile::remove(ui->lineEditFilePath->text() + "/" +ui->lineEditFileName->text()) ;

	this->hide();
}

createfile::~createfile()
{
	delete ui;
}
