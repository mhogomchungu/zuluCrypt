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
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"
#include "filetask.h"

#include "dialogmsg.h"

#include <QFileDialog>
#include <QFile>

#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>

createfile::createfile( QWidget * parent) :QDialog( parent ),m_ui( new Ui::createfile )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_msg = new DialogMsg( this ) ;

	m_ui->progressBar->setMinimum( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setValue( 0 ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;

	m_task = NULL ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) )  ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT(pbOpenFolder() ) ) ;
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->lineEditFileName,SIGNAL( textChanged( QString ) ),this,SLOT(fileTextChange( QString ) ) ) ;
}

void createfile::fileTextChange( QString txt )
{
	QString p = m_ui->lineEditFilePath->text() ;

	if( p.isEmpty() ){
		QString x = QDir::homePath() + QString( "/" ) + txt.split( "/" ).last() ;
		m_ui->lineEditFilePath->setText( x ) ;
		return ;
	}

	int i = p.lastIndexOf( "/" ) ;
	if( i == -1 ){
		return ;
	}
	p = p.mid( 0,i ) + QString( "/" ) + txt.split( "/" ).last() ;

	m_ui->lineEditFilePath->setText( p ) ;
}

void createfile::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	if( m_task != NULL ){
		return ;
	}
	this->pbCancel() ;
}

void createfile::enableAll()
{
	m_ui->lineEditFileName->setEnabled( true ) ;
	m_ui->lineEditFilePath->setEnabled( true ) ;
	m_ui->lineEditFileSize->setEnabled( true ) ;
	m_ui->pbOpenFolder->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->label_4->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
}

void createfile::disableAll()
{
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->lineEditFileName->setEnabled( false ) ;
	m_ui->lineEditFilePath->setEnabled( false ) ;
	m_ui->lineEditFileSize->setEnabled( false ) ;
	m_ui->comboBox->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label_3->setEnabled( false ) ;
	m_ui->label_4->setEnabled( false ) ;
}

void createfile::showUI()
{
	this->enableAll() ;
	m_ui->comboBox->setCurrentIndex( 1 ) ;
	m_ui->lineEditFileName->clear() ;
	m_ui->lineEditFilePath->setText( QDir::homePath() + QString( "/") ) ;
	m_ui->lineEditFileSize->clear() ;
	m_ui->progressBar->setValue( 0 ) ;
	m_ui->lineEditFileName->setFocus() ;

	/*
	 * RANDOM_SOURCE is defined at createfilethread.h
	 */
	if( RANDOM_SOURCE == 0 ){
		this->setWindowTitle( tr( "1/2 create a container file" ) ) ;
	}else{
		this->setWindowTitle( tr( "create a container file" ) ) ;
	}
	this->show() ;
}

void createfile::pbCreate()
{
	DialogMsg msg( this ) ;

	QString fileName = m_ui->lineEditFileName->text() ;
	QString filePath = m_ui->lineEditFilePath->text() ;
	QString fileSize = m_ui->lineEditFileSize->text() ;

	if( fileName.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "file name field is empty" ) ) ;
	}
	if( filePath.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "file path field is empty" ) ) ;
	}
	if( fileSize.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "file size field is empty" ) ) ;
	}

	bool test ;

	fileSize.toInt( &test ) ;

	if( test == false ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Illegal character in the file size field.Only digits are allowed" ) ) ;
	}

	m_path = filePath ;

	if( utility::exists( m_path ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "file with the same name and at the destination folder already exist" ) ) ;
	}
	if( !utility::canCreateFile( m_path ) ){
		msg.ShowUIOK( tr( "ERROR!" ),tr( "you dont seem to have writing access to the destination folder" ) ) ;
		m_ui->lineEditFilePath->setFocus() ;
		return ;
	}

	/*
	 * BLOCK_SIZE is defined in createfilethread.h
	 */
	switch( m_ui ->comboBox->currentIndex() ){
		case 0 :m_fileSize = fileSize.toULongLong() * BLOCK_SIZE ;
			break ;
		case 1 :m_fileSize = fileSize.toULongLong() * BLOCK_SIZE * BLOCK_SIZE ;
			break ;
		case 2 :m_fileSize = fileSize.toULongLong() * BLOCK_SIZE * BLOCK_SIZE  * BLOCK_SIZE ;
			break ;
	}

	if( m_fileSize < 3145728 ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "container file must be bigger than 3MB" ) ) ;
	}

	this->disableAll() ;

	m_task = new FileTask( m_path,m_fileSize ) ;

	connect( m_task,SIGNAL( doneCreatingFile() ),this,SLOT( doneCreatingFile() ) ) ;
	connect( m_task,SIGNAL( progress( int ) ),this,SLOT( progress( int ) ) ) ;
	connect( this,SIGNAL( cancelOperation()),m_task,SLOT( cancelOperation() ) ) ;
	connect( m_task,SIGNAL( exitStatus( int ) ),this,SLOT( exitStatus( int ) ) ) ;

	m_task->start() ;
}

void createfile::exitStatus( int status )
{
	m_task = NULL ;

	FileTask::status st = FileTask::status( status ) ;
	if( st == FileTask::cancelled ){
		QFile::remove( m_path ) ;
		return HideUI() ;
	}else if( st == FileTask::success ){
		if( m_msg->isVisible() ){
			m_msg->HideUI() ;
		}
	}else if( st == FileTask::openMapperFailed ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "could not open cryptographic back end to generate random data" ) ) ;
	}else{
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "could not open cryptographic back end to generate random data" ) ) ;
	}

	emit fileCreated( m_path ) ;
	this->HideUI() ;
}

void createfile::pbCancel()
{
	if( m_task == NULL ){
		return this->HideUI() ;
	}
	QString x = tr( "terminating file creation process" ) ;
	QString y = tr( "are you sure you want to stop file creation process?" ) ;
	if( m_msg->ShowUIYesNoDefaultNo( x,y ) == QMessageBox::Yes ){
		emit cancelOperation() ;
	}
}

void createfile::HideUI()
{
	emit HideUISignal() ;
	this->hide() ;
}

void createfile::progress( int p )
{
	m_ui->progressBar->setValue( p ) ;
}

void createfile::doneCreatingFile()
{
	m_ui->progressBar->setValue( 0 ) ;
	this->setWindowTitle( tr( "2/2 write random data to a container file" ) ) ;
}

void createfile::pbOpenFolder()
{
	QString p = tr( "Select Path to where the file will be created" ) ;
	QString q = QDir::homePath() ;
	QString Z = QFileDialog::getExistingDirectory( this,p,q,QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + QString( "/" ) + m_ui->lineEditFilePath->text().split( "/" ).last() ;
		m_ui->lineEditFilePath->setText( Z ) ;
	}
}

createfile::~createfile()
{
	delete m_ui;
}
