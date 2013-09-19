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

#include "kwalletconfiginput.h"
#include "ui_kwalletconfiginput.h"

#include <QDir>
#include <QFileDialog>
#include <QCloseEvent>

#include "dialogmsg.h"
#include "utility.h"
#include "openvolume.h"
#include <QDebug>
kwalletconfiginput::kwalletconfiginput( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::kwalletconfiginput )
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
}

kwalletconfiginput::~kwalletconfiginput()
{
	delete m_ui ;
}

void kwalletconfiginput::ShowUI()
{
	this->show() ;
}

void kwalletconfiginput::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void kwalletconfiginput::pbAdd()
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

	this->hide() ;
	emit add( volumeID,comment,key ) ;
	/*
	 * we dont hide and delete here because if the adding of an entry in the wallet takes too long to complete,the
	 * UI freeze will be noticeable.
	 */
	//this->HideUI() ;
}

void kwalletconfiginput::slotCancel()
{
	emit cancel() ;
	this->HideUI() ;
}

void kwalletconfiginput::pbImageFilePath()
{
	QString x = QFileDialog::getOpenFileName( this,tr( "select a luks volume" ),QDir::homePath(),0 ) ;

	if( x.isEmpty() ){
		return ;
	}

	QString z = utility::getUUIDFromPath( x ) ;

	if( z.isEmpty() ){
		this->setvolumeID( x ) ;
	}else{
		this->setvolumeID( z ) ;
	}
}

void kwalletconfiginput::pbVolumePath()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( setvolumeID( QString ) ) ) ;
	op->showEncryptedOnly() ;
	op->ShowAllPartitions() ;
}

void kwalletconfiginput::setvolumeID( QString id )
{
	m_ui->lineEditVolumeID->setText( id ) ;
	this->focus() ;
}

void kwalletconfiginput::focus()
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

void kwalletconfiginput::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	emit cancel() ;
	this->HideUI() ;
}
