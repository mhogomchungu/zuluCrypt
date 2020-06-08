/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
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

#include "luksdeletekey.h"
#include "zulucrypt.h"
#include "../zuluCrypt-cli/constants.h"
#include "utility.h"

#include <QObject>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QIcon>
#include <QKeyEvent>

#include "ui_openvolume.h"
#include "openvolume.h"
#include "ui_luksdeletekey.h"
#include "task.hpp"
#include "dialogmsg.h"
#include "plugin.h"

luksdeletekey::luksdeletekey( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::luksdeletekey )
{
	m_isWindowClosable = true ;
	m_ui->setupUi( this ) ;

	m_label.setOptions( m_ui->label_2,m_ui->pushButton ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->lineEditVolumePath->clear() ;

	connect( m_ui->pushButtonDelete,SIGNAL( clicked() ),this,SLOT( pbDelete() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pushButtonOpenKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenKeyFile() ) ) ;
	connect( m_ui->pushButtonOpenVolume,SIGNAL( clicked() ),this,SLOT( pbOpenVolume() ) ) ;
	connect( m_ui->pushButtonOpenPartition,SIGNAL( clicked() ),this,SLOT( pbOpenPartition() ) ) ;

	connect( m_ui->cbKey,SIGNAL( activated( int ) ),this,SLOT( Key( int ) ) ) ;

	this->Key( 0 ) ;

	m_ui->cbKey->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	this->installEventFilter( this ) ;
}

bool luksdeletekey::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void luksdeletekey::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	if(  m_isWindowClosable ){

		this->pbCancel() ;
	}
}

void luksdeletekey::Key( int e )
{
	auto _key_ui = [ this ](){

		m_ui->lineEditPassphrase->setToolTip( tr( "Enter a key" ) ) ;
		m_ui->labelPassphrase->setText( tr( "Password" ) ) ;
		m_ui->lineEditPassphrase->setEchoMode( QLineEdit::Password ) ;
		m_ui->lineEditPassphrase->clear() ;
		m_ui->pushButtonOpenKeyFile->setEnabled( false ) ;
		m_ui->pushButtonOpenKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
		m_ui->lineEditPassphrase->setFocus() ;
		m_ui->lineEditPassphrase->setEnabled( true ) ;
	} ;

	if( e == 0 || e == 3 ){

		_key_ui() ;

	}else if( e == 1 ){

		m_ui->lineEditPassphrase->setToolTip( tr( "Enter a path to a keyfile location" ) ) ;
		m_ui->labelPassphrase->setText( tr( "KeyFile path" ) ) ;
		m_ui->lineEditPassphrase->setEchoMode( QLineEdit::Normal ) ;
		m_ui->lineEditPassphrase->clear() ;
		m_ui->pushButtonOpenKeyFile->setEnabled( true ) ;
		m_ui->pushButtonOpenKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
		m_ui->lineEditPassphrase->setEnabled( true ) ;
	}else{
		_key_ui() ;

		m_ui->lineEditPassphrase->setEnabled( false ) ;

		plugin::instance( this,plugins::plugin::hmac_key,[ this ]( const QString& key ){

			m_ui->lineEditPassphrase->setText( key ) ;

			if( key.isEmpty() ){

				m_ui->cbKey->setCurrentIndex( 0 ) ;
				this->Key( 0 ) ;
			}
		} ) ;
	}
}

void luksdeletekey::pbOpenKeyFile()
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Key File With A Passphrase To Delete" ),utility::homePath() ) ;

	m_ui->lineEditPassphrase->setText( Z ) ;

	if( m_ui->lineEditVolumePath->text().isEmpty() ){

		m_ui->lineEditVolumePath->setFocus() ;
	}else{
		m_ui->pushButtonDelete->setFocus() ;
	}
}

void luksdeletekey::ShowUI()
{
	this->enableAll() ;

	m_ui->labelPassphrase->setText( tr( "Password" ) ) ;

	if( m_ui->lineEditVolumePath->text().isEmpty() ){

		m_ui->lineEditVolumePath->setFocus() ;
	}else{
		m_ui->lineEditPassphrase->setFocus() ;
	}

	m_ui->pushButtonOpenPartition->setIcon( QIcon( ":/partition.png" ) ) ;
	m_ui->pushButtonOpenVolume->setIcon( QIcon( ":/file.png" ) ) ;

	this->show() ;
}

void luksdeletekey::ShowUI( const QString& path )
{
	m_ui->lineEditVolumePath->setText( path ) ;
	this->ShowUI() ;
}

void luksdeletekey::disableAll()
{
	m_ui->cbKey->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->labelPassphrase->setEnabled( false ) ;
	m_ui->lineEditPassphrase->setEnabled( false ) ;
	m_ui->lineEditVolumePath->setEnabled( false ) ;
	m_ui->pushButtonCancel->setEnabled( false ) ;
	m_ui->pushButtonDelete->setEnabled( false ) ;
	m_ui->pushButtonOpenKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenPartition->setEnabled( false ) ;
	m_ui->pushButtonOpenVolume->setEnabled( false ) ;
}

void luksdeletekey::enableAll()
{
	m_ui->cbKey->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->labelPassphrase->setEnabled( true ) ;
	m_ui->lineEditPassphrase->setEnabled( true ) ;
	m_ui->lineEditVolumePath->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonDelete->setEnabled( true ) ;
	m_ui->pushButtonOpenPartition->setEnabled( true ) ;
	m_ui->pushButtonOpenVolume->setEnabled( true ) ;

	if( m_ui->cbKey->currentIndex() == 1 ){

		m_ui->pushButtonOpenKeyFile->setEnabled( true ) ;
	}
}

void luksdeletekey::pbCancel()
{
	this->HideUI() ;
}

void luksdeletekey::pbOpenPartition()
{
	openvolume::instance( this,false ).showLuksOnly().ShowAllPartitions( [ this ]( const QString& e ){

		this->ShowUI( e ) ;
	} ) ;
}

void luksdeletekey::pbDelete()
{
	DialogMsg msg( this ) ;

	m_volumePath = utility::resolvePath( m_ui->lineEditVolumePath->text() ) ;

	this->disableAll() ;

	utility2::raii raii( [ this ](){ this->enableAll() ; } ) ;

	if( m_volumePath.isEmpty() ){

		msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
	}else{
		if( utility::requireSystemPermissions( m_volumePath ) ){

			if( !utility::enablePolkit( utility::background_thread::False ) ){

				return msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to enable polkit support" ) ) ;
			}
		}

		m_volumePath.replace( "\"","\"\"\"" ) ;

		raii.cancel() ;

		this->deleteKey( utility::luksEmptySlots( m_volumePath ).await() ) ;
	}
}

void luksdeletekey::deleteKey( const QStringList& l )
{
	if( l.isEmpty() ){

		m_label.show( tr( "Volume is not a luks volume" ) ) ;

		return this->enableAll() ;

	}else if( l.first().toInt() == 1 ){

		auto s = tr( "There is only one last key in the volume." ) ;
		s = s + tr( "\nDeleting it will make the volume unopenable and lost forever." ) ;
		s = s + tr( "\nAre you sure you want to delete this key?" ) ;

		if( DialogMsg( this ).ShowUIYesNoDefaultNo( tr( "WARNING" ),s ) == QMessageBox::No ){

			return this->enableAll() ;
		}
	}else{
		//auto s = tr( "Are you sure you want to delete a key from this volume?" ) ;

		//if( DialogMsg( this ).ShowUIYesNoDefaultNo( tr( "WARNING" ),s ) == QMessageBox::No ){

		//	return this->enableAll() ;
		//}
	}

	auto keyNumber = l.first().toInt() - 1 ;
	auto totalKeys = l.at( 1 ) ;

	QString keypath ;

	if( m_ui->cbKey->currentIndex() == 1 ){

		keypath = utility::resolvePath( m_ui->lineEditPassphrase->text() ).replace( "\"","\"\"\"" ) ;
	}else{
		keypath = utility::keyPath() ;

		auto key = m_ui->lineEditPassphrase->text() ;

		if( m_ui->cbKey->currentIndex() == 3 ){

			auto m = utility::yubiKey( key ) ;

			if( m.has_value() ){

				key = m.value() ;
			}else{
				m_label.show( tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
				return this->enableAll() ;
			}
		}

		utility::keySend( keypath,key ) ;
	}

	auto r = "%1 -k -r -d \"%2\" -f \"%3\"" ;

	auto exe = utility::appendUserUID( r ).arg( ZULUCRYPTzuluCrypt,m_volumePath,keypath ) ;

	m_isWindowClosable = false ;

	auto e = utility::Task::run( exe ).await() ;

	m_isWindowClosable = true ;

	QString success ;

	switch( e.exitCode() ){
		case 0 :
			success = tr( "Key removed successfully.\n%1 / %2 slots are now in use" ).arg( QString::number( keyNumber ) ).arg( totalKeys ) ;
			m_label.show( success ) ;
			return this->HideUI() ;
		case 2 : m_label.show( tr( "There is no key in the volume that match the presented key" ) ) ;				break ;
		case 3 : m_label.show( tr( "Could not open the volume" ) ) ;								break ;
		case 4 : m_label.show( tr( "Insufficient privilege to open a system device,\
only root user or members of group zulucrypt can do that" ) ) ;											break ;
		case 5 : m_label.show( tr( "Could not open the volume in write mode" ) ) ;						break ;
		case 6 : m_label.show( tr( "Insufficient memory to hold your response" ) ) ;						break ;
		case 7 : m_label.show( tr( "Operation terminated per user request" ) ) ;							break ;
		case 8 : m_label.show( tr( "Can not get passphrase in silent mode" ) ) ;							break ;
		case 9 : m_label.show( tr( "Insufficient memory to hold passphrase" ) ) ;						break ;
		case 10: m_label.show( tr( "One or more required argument(s) for this operation is missing" ) ) ;			break ;
		case 11: m_label.show( tr( "Keyfile does not exist" ) ) ;								break;
		case 12: m_label.show( tr( "Could not get enough memory to open the key file") ) ;					break ;
		case 13: m_label.show( tr( "Insufficient privilege to open key file for reading" ) ) ;					break ;
		case 14: m_label.show( tr( "Could not get a key from a socket" ) ) ;							break ;
		case 110:m_label.show( tr( "Can not find a partition that match presented UUID" ) ) ;					break ;
		default: m_label.show( tr( "Error Code: %1\n--\nStdOut: %2\n--\nStdError: %3").arg( QString::number( e.exitCode() ),QString( e.stdError() ),QString( e.stdOut() ) ) ) ;
	}

	this->enableAll() ;

	if( e.exitCode() == 2 ){

		this->Key( 0 ) ;
		m_ui->cbKey->setCurrentIndex( 0 ) ;
	}
}

void luksdeletekey::pbOpenVolume()
{
	auto Z = QFileDialog::getOpenFileName( this,tr( "Volume Path" ),utility::homePath() ) ;

	m_ui->lineEditVolumePath->setText( Z ) ;

	if( m_ui->lineEditPassphrase->text().isEmpty() ){

		m_ui->lineEditPassphrase->setFocus() ;
	}else{
		m_ui->pushButtonDelete->setFocus() ;
	}
}

void luksdeletekey::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

luksdeletekey::~luksdeletekey()
{
	delete m_ui ;
}
