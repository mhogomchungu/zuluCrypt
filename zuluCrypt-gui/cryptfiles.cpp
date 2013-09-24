/*
 *
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cryptfiles.h"
#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QKeyEvent>

#include <QThreadPool>
#include <QFileDialog>
#include <QDir>

#include "ui_cryptfiles.h"
#include "utility.h"
#include "openvolume.h"
#include "crypttask.h"
#include "dialogmsg.h"
#include "socketsendkey.h"

cryptfiles::cryptfiles( QWidget * parent ) :QDialog( parent ),m_ui( new Ui::cryptfiles )
{
	m_ui->setupUi( this ) ;
	this->setFont( parent->font() ) ;
	this->setFixedSize( this->size() ) ;

	m_ui->progressBar->setMinimum( 0 ) ;
	m_ui->progressBar->setMaximum( 100 ) ;
	m_ui->progressBar->setValue( 0 ) ;

	m_ui->pbOpenFolder->setIcon( QIcon( QString( ":/folder.png" ) ) ) ;
	m_ui->pushButtonFile->setIcon( QIcon( QString( ":/file.png" ) ) ) ;

	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreate() ) ) ;
	connect( m_ui->pushButtonFile,SIGNAL( clicked() ),this,SLOT( pbOpenFile() ) ) ;
	connect( m_ui->pbOpenFolder,SIGNAL( clicked() ),this,SLOT( pbOpenFolder() ) ) ;
	connect( m_ui->rbKey,SIGNAL( clicked() ),this,SLOT( rbKeyChecked() ) ) ;
	connect( m_ui->rbKeyFile,SIGNAL( clicked() ),this,SLOT( rbKeyFileChecked() ) ) ;
	connect( m_ui->lineEditSourcePath,SIGNAL( textChanged( QString ) ),this,SLOT( sourceTextChanged( QString ) ) ) ;
	connect( m_ui->pushButtonKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	this->rbKeyChecked() ;

	m_OperationInProgress = false ;

	m_ui->rbKey->setChecked( true ) ;

	m_ui->lineEditDestinationPath->setText( QDir::homePath() + QString( "/" ) ) ;
	m_ui->lineEditPass_2->setEchoMode( QLineEdit::Password ) ;

	m_ui->lineEditSourcePath->setFocus() ;

}

void cryptfiles::rbKeyChecked()
{
	m_ui->lineEditPass_1->setToolTip( tr( "enter a key" ) ) ;
	m_ui->pushButtonKeyFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) ) ;
	m_ui->pushButtonKeyFile->setEnabled( false ) ;
	m_ui->lineEditPass_1->clear() ;
	m_ui->lineEditPass_2->clear() ;
	m_ui->lineEditPass_1->setEchoMode( QLineEdit::Password ) ;
	m_ui->lineEditPass_1->setFocus() ;
	m_ui->labelKey->setText( tr( "key" ) ) ;
	m_ui->labelKey2->setText( tr( "repeat key" ) ) ;

	if( m_operation == QString( "-E" ) ){
		m_ui->labelKey2->setEnabled( true ) ;
		m_ui->lineEditPass_2->setEnabled( true ) ;
	}

}

void cryptfiles::rbKeyFileChecked()
{
	m_ui->lineEditPass_1->setToolTip( tr( "enter a path to a keyfile location" ) ) ;
	m_ui->labelKey->setText( tr( "keyfile path" ) ) ;
	m_ui->pushButtonKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	m_ui->pushButtonKeyFile->setEnabled( true ) ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->lineEditPass_1->clear() ;
	m_ui->lineEditPass_2->clear() ;
	m_ui->lineEditPass_1->setEchoMode( QLineEdit::Normal ) ;
	m_ui->lineEditPass_1->setFocus() ;

}

void cryptfiles::sourceTextChanged( QString source )
{
	QString dest ;

	if( m_operation == QString( "-E" ) ){
		dest = source.split( "/" ).last() + QString( ".zc" ) ;
	}else{
		dest = source.split( "/" ).last() ;
	}

	QStringList p = m_ui->lineEditDestinationPath->text().split( "/" ) ;

	int size = p.size() ;
	QString path = QString( "" ) ;
	for( int i = 0 ; i < size - 1 ; i++ ){
		path += p.at( i ) + QString( "/" ) ;
	}
	path += dest ;

	if( m_operation == QString( "-D" ) ){
		if( path.endsWith( QString( ".zc" ) ) ){
			path = path.mid( 0,path.size() - 3 ) ;
		}
	}

	m_ui->lineEditDestinationPath->setText( path ) ;

}

void cryptfiles::encrypt()
{
	m_operation = QString( "-E" ) ;
	this->setWindowTitle( tr( "create encrypted version of a file" ) ) ;
	this->show() ;
}

void cryptfiles::decrypt()
{
	m_operation = QString( "-D" ) ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	this->setWindowTitle( tr( "create decrypted version of an encrypted file" ) ) ;
	this->show() ;
}

void cryptfiles::closeEvent( QCloseEvent *e )
{
	e->ignore() ;
	this->HideUI() ;
}

void cryptfiles::pbCancel()
{
	this->HideUI() ;
}

void cryptfiles::HideUI()
{
	if( m_OperationInProgress ){
		m_task->terminate() ;
	}else{
		emit this->HideUISignal() ;
		this->hide() ;
	}
}

void cryptfiles::enableAll()
{
	if( m_operation == QString( "-E" ) ){
		m_ui->labelKey2->setEnabled( true ) ;
		m_ui->lineEditPass_2->setEnabled( true ) ;
	}

	m_ui->labelKey->setEnabled( true ) ;
	m_ui->lineEditPass_1->setEnabled( true ) ;

	m_ui->labelDestinationPath->setEnabled( true ) ;
	m_ui->labelSourcePath->setEnabled( true ) ;
	m_ui->lineEditDestinationPath->setEnabled( true ) ;
	m_ui->lineEditSourcePath->setEnabled( true ) ;
	m_ui->pbCreate->setEnabled( true ) ;
	m_ui->pbOpenFolder->setEnabled( true ) ;
	m_ui->pushButtonFile->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->rbKey->setEnabled( true ) ;
	m_ui->rbKeyFile->setEnabled( true ) ;
	m_ui->labelProgressBar->setEnabled( true ) ;
}

void cryptfiles::disableAll()
{
	m_ui->lineEditPass_1->setEnabled( false ) ;
	m_ui->lineEditPass_2->setEnabled( false ) ;
	m_ui->labelKey2->setEnabled( false ) ;
	m_ui->labelKey->setEnabled( false ) ;
	m_ui->labelDestinationPath->setEnabled( false ) ;
	m_ui->labelSourcePath->setEnabled( false ) ;
	m_ui->lineEditDestinationPath->setEnabled( false ) ;
	m_ui->lineEditSourcePath->setEnabled( false ) ;
	m_ui->pbCreate->setEnabled( false ) ;
	m_ui->pbOpenFolder->setEnabled( false ) ;
	m_ui->pushButtonFile->setEnabled( false ) ;
	m_ui->rbKey->setEnabled( false ) ;
	m_ui->rbKeyFile->setEnabled( false ) ;
	m_ui->labelProgressBar->setEnabled( false ) ;
	m_ui->pushButtonCancel->setEnabled( false ) ;
}

void cryptfiles::pbCreate()
{
	DialogMsg msg( this ) ;

	QString source = utility::resolvePath( m_ui->lineEditSourcePath->text() ) ;

	if( source.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "path to source field is empty" ) ) ;
	}
	QString dest = utility::resolvePath( m_ui->lineEditDestinationPath->text() ) ;

	if( !utility::exists( source ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to source file" ) ) ;
	}
	if( utility::exists( dest ) ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "destination path already taken" ) ) ;
	}
	QString key_1 = m_ui->lineEditPass_1->text() ;
	QString key_2 = m_ui->lineEditPass_2->text() ;

	QString keySource ;
	if( m_ui->rbKey->isChecked() ){
		if( key_1.isEmpty() ){
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "first key field is empty" ) ) ;
		}
		if( m_operation == QString( "-E" ) ){
			if( key_2.isEmpty() ){
				return msg.ShowUIOK( tr( "ERROR!" ),tr( "second key field is empty" ) ) ;
			}
			if( key_1 != key_2 ){
				return msg.ShowUIOK( tr( "ERROR!" ),tr( "keys do not match" ) ) ;
			}
		}

		QString sockpath = socketSendKey::getSocketPath() ;
		socketSendKey * sk = new socketSendKey( this,sockpath,key_1.toAscii() ) ;
		key_1 = sockpath ;
		sk->sendKey() ;
	}else{
		if( !utility::exists( key_1 ) ){
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to key file" ) ) ;
		}
	}

	keySource = QString( "-f" ) ;

	this->disableAll() ;

	m_OperationInProgress = true ;

	m_task = new CryptTask( source,dest,keySource,key_1,m_operation ) ;

	connect( m_task,SIGNAL( complete( int ) ),this,SLOT( threadExitStatus( int ) ) ) ;
	connect( m_task,SIGNAL( progressUpdate( int ) ),this,SLOT( progressBarUpdate( int ) ) ) ;
	connect( m_task,SIGNAL( titleUpdate( QString ) ),this,SLOT( titleUpdate( QString ) ) ) ;
	connect( m_task,SIGNAL( enableCancel() ),this,SLOT( enableCancel() ) ) ;
	connect( m_task,SIGNAL( disableCancel() ),this,SLOT( disableCancel() ) ) ;

	m_task->start() ;
}

void cryptfiles::disableCancel()
{
	m_ui->pushButtonCancel->setEnabled( false ) ;
}

void cryptfiles::enableCancel()
{
	m_ui->pushButtonCancel->setEnabled( true ) ;
}

void cryptfiles::titleUpdate( QString title )
{
	this->setWindowTitle( title ) ;
}

void cryptfiles::progressBarUpdate( int i )
{
	m_ui->progressBar->setValue( i ) ;
}

void cryptfiles::pbOpenFolder( void )
{
	QString p = tr( "Select Path to put destination file" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,p,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( Z.isEmpty() ){
		Z = QDir::homePath() ;
	}
	QString path ;
	if( m_operation == QString( "-E" ) ){
		path = Z + QString( "/" ) + m_ui->lineEditSourcePath->text().split( "/" ).last() + QString( ".zc" ) ;
	}else{
		path = Z + QString( "/" ) + m_ui->lineEditSourcePath->text().split( "/" ).last() ;
		path.chop( 3 ) ;
	}

	m_ui->lineEditDestinationPath->setText( path ) ;
	if( m_ui->lineEditSourcePath->text().isEmpty() ){
		m_ui->lineEditSourcePath->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void cryptfiles::pbOpenFile()
{
	QString Z ;
	if( m_operation == QString( "-E" ) ){
		Z = QFileDialog::getOpenFileName( this,tr( "select a file you want to encrypt" ),QDir::homePath(),0 ) ;
	}else{
		QString x = tr( "zuluCrypt encrypted files ( *.zc ) ;; All Files ( * )" ) ;
		Z = QFileDialog::getOpenFileName( this,tr( "select a file you want to decrypt" ),QDir::homePath(),x ) ;
	}
	m_ui->lineEditSourcePath->setText( Z ) ;
	m_ui->lineEditPass_1->setFocus() ;
}

void cryptfiles::pbKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "select a keyfile" ),QDir::homePath(),0 ) ;

	m_ui->lineEditPass_1->setText( Z ) ;
	if( m_ui->lineEditSourcePath->text().isEmpty() ){
		m_ui->lineEditSourcePath->setFocus() ;
	}else{
		m_ui->pbCreate->setFocus() ;
	}
}

void cryptfiles::threadExitStatus( int st )
{
	DialogMsg msg( this ) ;

	m_OperationInProgress = false ;

	switch( st ){
		case 0 : msg.ShowUIOK( tr( "SUCCESS" ),tr( "encrypted file created successfully" ) ) ;
			 return this->HideUI() ;
		case 1 : msg.ShowUIOK( tr( "SUCCESS" ),tr( "decrypted file created successfully" ) )	;
			 return this->HideUI() ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open keyfile for reading" ) )				; break ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "missing key source" ) )						; break ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open encryption routines" ) )				; break ;
		case 5 : msg.ShowUIOK( tr( "ERROR!" ),tr( "file or folder already exist at destination address" ) )		; break ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to source" ) )						; break ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not resolve path to destination file" ) )			; break ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "keys do not match" ) )						; break ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "required argument is missing" ) )					; break ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to create destination file" ) )		; break ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "presented key did not match the encryption key" ) )			; break ;
		case 12: msg.ShowUIOK( tr( "INFO!" ),tr( "operation terminated per user request" ) ) ;
			 return this->HideUI() ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open source file for reading" ) )		; break ;
		case 1000: msg.ShowUIOK( tr( "WARNING"),tr( "decrypted file created successfully but md5 checksum failed,file maybe corrupted" ) ) ;
		return this->HideUI() ;
	}
	this->enableAll() ;
	if( st == 11 || st == 2 ){
		m_ui->lineEditPass_1->clear() ;
		m_ui->lineEditPass_1->setFocus() ;
	}
}

cryptfiles::~cryptfiles()
{
	delete m_ui;
}
