/*
 *  Copyright ( c ) 2012
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

#include "walletconfiginput.h"
#include "ui_walletconfiginput.h"

#include <QDir>
#include <QFileDialog>
#include <QCloseEvent>

#include "dialogmsg.h"
#include "utility.h"
#include "openvolume.h"
#include <QDebug>

walletconfiginput::walletconfiginput( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::walletconfiginput )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	if( parent ){
		this->setFont( parent->font() ) ;
	}

	m_ui->lineEditVolumeID->setEnabled( false ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
	m_ui->lineEditRepeatKey->setEchoMode( QLineEdit::Password ) ;

	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( slotCancel() ) ) ;
	connect( m_ui->pushButtonImageFile,SIGNAL( clicked() ),this,SLOT( pbImageFilePath() ) ) ;
	connect( m_ui->pushButtonVolume,SIGNAL( clicked() ),this,SLOT( pbVolumePath() ) ) ;

	m_ui->pushButtonImageFile->setIcon( QIcon( QString( ":/file.png" ) ) ) ;
	m_ui->pushButtonVolume->setIcon( QIcon( QString( ":/partition.png" ) ) ) ;

	this->installEventFilter( this ) ;
}

bool walletconfiginput::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

walletconfiginput::~walletconfiginput()
{
	delete m_ui ;
}

void walletconfiginput::ShowUI()
{
	this->show() ;
}

void walletconfiginput::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void walletconfiginput::pbAdd()
{
	QString volumeID  = m_ui->lineEditVolumeID->text() ;
	QString comment   = m_ui->lineEditComment->text()  ;
	QString key       = m_ui->lineEditKey->text()      ;
	QString repeatKey = m_ui->lineEditRepeatKey->text() ;

	if( volumeID.isEmpty() || key.isEmpty() || repeatKey.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "atleast one required field is empty" ) ) ;
		return ;
	}

	if( key != repeatKey ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "passphrases do not match" ) ) ;
		return ;
	}

	if( comment.isEmpty() ){
		comment = QString( "Nil" ) ;
	}

	emit add( volumeID,comment,key ) ;
	this->HideUI() ;
}

void walletconfiginput::slotCancel()
{
	emit cancel() ;
	this->HideUI() ;
}

void walletconfiginput::pbImageFilePath()
{
	QString x = QFileDialog::getOpenFileName( this,tr( "select a volume" ),QDir::homePath(),0 ) ;

	if( !x.isEmpty() ){
		this->setvolumeID( x ) ;
	}
}

void walletconfiginput::pbVolumePath()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( setvolumeID( QString ) ) ) ;
	op->showEncryptedOnly() ;
	op->ShowAllPartitions() ;
}

void walletconfiginput::setvolumeID( QString id )
{
	if( id.startsWith( QString( "UUID=") ) ){
		m_ui->lineEditVolumeID->setText( id ) ;
	}else{
		QString z = utility::getUUIDFromPath( id ) ;
		if( z.isEmpty() ){
			m_ui->lineEditVolumeID->setText( utility::getVolumeID( id ) ) ;
		}else{
			m_ui->lineEditVolumeID->setText( z ) ;
		}
	}

	this->focus() ;
}

void walletconfiginput::focus()
{
	if( m_ui->lineEditComment->text().isEmpty() ){
		m_ui->lineEditComment->setFocus() ;
	}else if( m_ui->lineEditKey->text().isEmpty() ){
		m_ui->lineEditKey->setFocus() ;
	}else if( m_ui->lineEditRepeatKey->text().isEmpty() ){
		m_ui->lineEditRepeatKey->setFocus() ;
	}else{
		m_ui->pushButtonAdd->setFocus() ;
	}
}

void walletconfiginput::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	emit cancel() ;
	this->HideUI() ;
}
