/*
 *
 *  Copyright (c) 2011
 *  name : Francis Banyikwa
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

#include "dialogmsg.h"

#include <QFileDialog>
#include <QFile>

#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>

createfile::createfile( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::createfile )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->progressBar->setMinimum( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setValue( 0 ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( ":/folder.png" ) ) ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) )  ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT(pbOpenFolder() ) ) ;
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->lineEditFileName,SIGNAL( textChanged( QString ) ),this,SLOT(fileTextChange( QString ) ) ) ;

	connect( this,SIGNAL( sendProgress( int ) ),this,SLOT( setProgress( int ) ) ) ;

	this->installEventFilter( this ) ;

	this->setWindowTitle( tr( "Create A Container File" ) ) ;

	m_running = false ;
}

bool createfile::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->pbCancel() ;
		return true ;
	}else{
		return false ;
	}
}

void createfile::fileTextChange( QString txt )
{
	QString p = m_ui->lineEditFilePath->text() ;

	if( p.isEmpty() ){
		QString x = utility::homePath() + "/" + txt.split( "/" ).last() ;
		m_ui->lineEditFilePath->setText( x ) ;
		return ;
	}

	int i = p.lastIndexOf( "/" ) ;
	if( i == -1 ){
		return ;
	}
	p = p.mid( 0,i ) + "/" + txt.split( "/" ).last() ;

	m_ui->lineEditFilePath->setText( p ) ;
}

void createfile::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
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
	m_ui->lineEditFilePath->setText( utility::homePath() + "/" ) ;
	m_ui->lineEditFileSize->clear() ;
	m_ui->progressBar->setValue( 0 ) ;
	m_ui->lineEditFileName->setFocus() ;
	this->show() ;
}

void createfile::pbCreate()
{
	DialogMsg msg( this ) ;

	QString fileName = m_ui->lineEditFileName->text() ;
	QString filePath = m_ui->lineEditFilePath->text() ;
	QString fileSize = m_ui->lineEditFileSize->text() ;

	if( fileName.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File name field is empty" ) ) ;
	}
	if( filePath.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File path field is empty" ) ) ;
	}
	if( fileSize.isEmpty()){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File size field is empty" ) ) ;
	}

	bool test ;

	fileSize.toInt( &test ) ;

	if( test == false ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Illegal character in the file size field.Only digits are allowed" ) ) ;
	}

	if( utility::pathExists( filePath ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "File with the same name and at the destination folder already exist" ) ) ;
	}
	if( !utility::canCreateFile( filePath ) ){
		msg.ShowUIOK( tr( "ERROR!" ),tr( "You dont seem to have writing access to the destination folder" ) ) ;
		m_ui->lineEditFilePath->setFocus() ;
		return ;
	}

	qulonglong size = 0 ;

	switch( m_ui ->comboBox->currentIndex() ){
	case 0 :size = fileSize.toULongLong() * 1024 ;
		break ;
	case 1 :size = fileSize.toULongLong() * 1024 * 1024 ;
		break ;
	case 2 :size = fileSize.toULongLong() * 1024 * 1024  * 1024 ;
		break ;
	}

	if( size < 3145728 ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Container file must be bigger than 3MB" ) ) ;
	}

	this->disableAll() ;

	QFile file( filePath ) ;

	if( !file.open( QIODevice::WriteOnly ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to create volume file" ) ) ;
	}

	emit sendProgress( 0 ) ;

	if( !file.resize( size ) ){
		QFile::remove( filePath ) ;
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to create volume file" ) ) ;
	}

	file.close() ;

	m_exit = false ;
	m_running = true ;

	int r = utility::clearVolume( filePath,&m_exit,[ this ]( int i ){ emit sendProgress( i ) ; } ).await() ;

	if( r == 5 ){
		msg.ShowUIOK( tr( "ERROR!" ),tr( "Operation terminated per user choice" ) ) ;
		QFile::remove( filePath ) ;
	}else if( r == 0 ){
		emit fileCreated( filePath ) ;
	}else{
		msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open cryptographic back end to generate random data" ) ) ;
		QFile::remove( filePath ) ;
	}

	m_running = false ;

	this->HideUI() ;
}

void createfile::pbCancel()
{
	if( m_running ){

		QString x = tr( "Terminating file creation process" ) ;
		QString y = tr( "Are you sure you want to stop file creation process?" ) ;

		DialogMsg msg( this ) ;

		if( msg.ShowUIYesNoDefaultNo( x,y ) == QMessageBox::Yes ){
			m_exit = true ;
		}
	}else{
		this->HideUI() ;
	}
}

void createfile::HideUI()
{
	emit HideUISignal() ;
	this->hide() ;
}

void createfile::setProgress( int p )
{
	m_ui->progressBar->setValue( p ) ;
}

void createfile::pbOpenFolder()
{
	QString p = tr( "Select Path to where the file will be created" ) ;
	QString q = utility::homePath() ;
	QString Z = QFileDialog::getExistingDirectory( this,p,q,QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + "/" + m_ui->lineEditFilePath->text().split( "/" ).last() ;
		m_ui->lineEditFilePath->setText( Z ) ;
	}
}

createfile::~createfile()
{
	delete m_ui ;
}
