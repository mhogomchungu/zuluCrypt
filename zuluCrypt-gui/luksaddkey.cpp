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

#include "ui_luksaddkey.h"
#include "luksaddkey.h"
#include "../zuluCrypt-cli/constants.h"
#include "zulucrypt.h"
#include "utility.h"

#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>
#include <QIcon>

luksaddkey::luksaddkey( QWidget * parent ) :
	QDialog( parent )
{
	m_ui = new Ui::luksaddkey() ;
	m_ui->setupUi( this );

	m_isWindowClosable = true ;

	m_ui->textEditPathToVolume->setText( QString( "" ) );

	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Password );
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password );

	this->setFixedSize( this->size() );
	this->setFont( parent->font() );

	connect( m_ui->pushButtonOpenFile,SIGNAL( clicked() ),this,SLOT( pbOpenFile() ) ) ;
	connect( m_ui->pushButtonOpenExistingKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenExisitingKeyFile() ) ) ;
	connect( m_ui->pushButtonOpenNewKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenNewKeyFile() ) ) ;
	connect( m_ui->pushButtonOpenPartition,SIGNAL( clicked() ),this,SLOT( pbOpenPartition( void ) ) ) ;
	connect( m_ui->pushButtonAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->radioButtonNewPassphrase,SIGNAL( toggled( bool ) ),this,SLOT( rbNewPassphrase() ) ) ;
	connect( m_ui->radioButtonNewPassphraseFromFile,SIGNAL( toggled( bool ) ),this,SLOT( rbNewPassphraseFromFile() ) ) ;
	connect( m_ui->radioButtonPassphraseinVolume,SIGNAL( toggled( bool ) ),this,SLOT( rbExistingPassphrase() ) ) ;
	connect( m_ui->radioButtonPassphraseInVolumeFromFile,SIGNAL( toggled( bool ) ),this,SLOT( rbExistingPassphraseFromFile() ) ) ;
	connect( m_ui->textEditPassphraseToAdd,SIGNAL( textChanged( QString ) ),this,SLOT( keyChanged( QString ) ) ) ;

	m_ui->lineEditReEnterPassphrase->setEchoMode( QLineEdit::Password );

	m_ui->radioButtonNewPassphrase->setChecked( true );
	m_ui->radioButtonPassphraseinVolume->setChecked( true );

	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false );
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false );

	m_ui->pushButtonOpenPartition->setIcon( QIcon( QString( ":/partition.png" ) ) );
	m_ui->pushButtonOpenFile->setIcon( QIcon( QString( ":/file.png" ) ) );

	m_keystrength = new keystrength() ;
}

void luksaddkey::keyChanged( QString key )
{
	if( m_ui->radioButtonNewPassphrase && m_keystrength->canCheckQuality() ){
		int st = m_keystrength->quality( key ) ;
		if(  st < 0 )
			this->setWindowTitle( tr( "passphrase quality: 0/100" ) ) ;
		else
			this->setWindowTitle( tr( "passphrase quality: %1/100" ).arg( st ) ) ;
	}else{
		this->setWindowTitle( tr( "add a key to a luks volume" ) ) ;
	}
}

void luksaddkey::closeEvent( QCloseEvent * e )
{
	e->ignore();
	if( m_isWindowClosable == true )
		HideUI();
}

void luksaddkey::ShowUI( QString path )
{
	m_ui->textEditPathToVolume->setText( path );
	this->ShowUI();
}

void luksaddkey::ShowUI()
{
	this->setFieldFocus();
	this->show();
}

void luksaddkey::setFieldFocus()
{
	if( m_ui->textEditPathToVolume->text().isEmpty() )
		m_ui->textEditPathToVolume->setFocus();
	else if( m_ui->textEditExistingPassphrase->text().isEmpty() )
		m_ui->textEditExistingPassphrase->setFocus();
	else if( m_ui->textEditPassphraseToAdd->text().isEmpty() )
		m_ui->textEditPassphraseToAdd->setFocus();
	else if( m_ui->radioButtonNewPassphrase->isChecked() )
		if( m_ui->lineEditReEnterPassphrase->text().isEmpty() )
			m_ui->lineEditReEnterPassphrase->setFocus();
		else
			m_ui->pushButtonAdd->setFocus();
	else
		m_ui->pushButtonAdd->setFocus();
}

void luksaddkey::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void luksaddkey::pbOpenExisitingKeyFile( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "existing key file" ),QDir::homePath(),0 );
	if( Z.isEmpty() == false )
		m_ui->textEditExistingPassphrase->setText( Z ) ;
	this->setFieldFocus();
}

void luksaddkey::pbOpenNewKeyFile( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "new key file" ),QDir::homePath(),0 );
	if( Z.isEmpty() == false )
		m_ui->textEditPassphraseToAdd->setText( Z ) ;
	this->setFieldFocus();
}

void luksaddkey::pbOpenFile( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "encrypted volume path" ),QDir::homePath(),0 );
	if( Z.isEmpty() == false )
		m_ui->textEditPathToVolume->setText( Z ) ;
	this->setFieldFocus();
}

void luksaddkey::pbOpenPartition( void )
{
	openpartition * openPartition = new openpartition( this );
	connect( openPartition,SIGNAL( clickedPartition( QString ) ),this,SLOT( ShowUI( QString ) ) );
	connect( openPartition,SIGNAL( HideUISignal() ),openPartition,SLOT( deleteLater() ) );
	connect( openPartition,SIGNAL( HideUISignal() ),this,SLOT( setFieldFocus() ) ) ;
	openPartition->ShowAllPartitions();
}

void luksaddkey::rbExistingPassphrase( void )
{
	m_ui->textEditExistingPassphrase->setToolTip( QString( "enter a key" ) );
	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Password );
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false );
	m_ui->labelExistingPassphrase->setText( tr( "key" ) ) ;
	m_ui->textEditExistingPassphrase->clear();
	m_ui->pushButtonOpenExistingKeyFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) );
	m_ui->textEditExistingPassphrase->setFocus();
}

void luksaddkey::rbExistingPassphraseFromFile( void )
{
	m_ui->textEditExistingPassphrase->setToolTip( QString( "enter a path to a keyfile location" ) );
	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Normal );
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( true );
	m_ui->labelExistingPassphrase->setText( tr( "keyfile path" ) ) ;
	m_ui->textEditExistingPassphrase->clear();
	m_ui->pushButtonOpenExistingKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->textEditExistingPassphrase->setFocus();

}

void luksaddkey::rbNewPassphrase( void )
{
	m_ui->textEditPassphraseToAdd->setToolTip( QString( "enter a key" ) );
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password );
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false );
	m_ui->labelNewPassphrase->setText( tr( "key" ) ) ;
	m_ui->textEditPassphraseToAdd->clear();
	m_ui->lineEditReEnterPassphrase->setEnabled( true ) ;
	m_ui->labelReEnterPassphrase->setEnabled( true );
	m_ui->pushButtonOpenNewKeyFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) );
	m_ui->textEditPassphraseToAdd->setFocus();
}

void luksaddkey::rbNewPassphraseFromFile()
{
	m_ui->textEditPassphraseToAdd->setToolTip( QString( "enter a path to a keyfile location" ) );
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Normal );
	m_ui->pushButtonOpenNewKeyFile->setEnabled( true );
	m_ui->labelNewPassphrase->setText( tr( "keyfile path" ) ) ;
	m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
	m_ui->lineEditReEnterPassphrase->clear() ;
	m_ui->labelReEnterPassphrase->setEnabled( false );
	m_ui->pushButtonOpenNewKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
	m_ui->textEditPassphraseToAdd->setFocus();
}

void luksaddkey::pbAdd( void )
{
	DialogMsg msg( this ) ;
	QString path = m_ui->textEditPathToVolume->text() ;
	m_volumePath = utility::resolvePath( path ) ;
	QString ExistingKey = m_ui->textEditExistingPassphrase->text() ;

	QString NewKey = m_ui->textEditPassphraseToAdd->text() ;
	QString NewKey_1 = m_ui->lineEditReEnterPassphrase->text() ;

	if( m_ui->radioButtonNewPassphraseFromFile->isChecked() ){
		if( m_volumePath.isEmpty() || ExistingKey.isEmpty() || NewKey.isEmpty() )
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) );
	}else{
		if( m_volumePath.isEmpty() || ExistingKey.isEmpty() || NewKey.isEmpty() || NewKey_1.isEmpty() )
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) );

		if( NewKey != NewKey_1 ){
			msg.ShowUIOK( tr( "ERROR!" ),tr( "keys do not match" ) );
			m_ui->textEditPassphraseToAdd->clear();
			m_ui->lineEditReEnterPassphrase->clear();
			m_ui->textEditPassphraseToAdd->setFocus();
			return ;
		}
	}

	ExistingKey.replace( "\"","\"\"\"" ) ;
	NewKey.replace( "\"","\"\"\"" ) ;

	QString existingPassType ;

	if ( m_ui->radioButtonPassphraseInVolumeFromFile->isChecked() ){
		ExistingKey = utility::resolvePath( ExistingKey );
		existingPassType = QString( "-u" ) ;
	}else{
		existingPassType = QString( "-u" ) ;
		ExistingKey = socketSendKey::getSocketPath() + QString( "-existingKey" ) ;

		socketSendKey * s = new socketSendKey( this,ExistingKey,m_ui->textEditExistingPassphrase->text().toAscii() ) ;
		s->sendKey();
	}

	QString newPassType ;
	if ( m_ui->radioButtonNewPassphraseFromFile->isChecked() ){
		NewKey = utility::resolvePath( NewKey );
		newPassType = QString( "-n" ) ;
	}else{
		newPassType = QString( "-n" ) ;

		NewKey = socketSendKey::getSocketPath() + QString( "-newKey" ) ;

		socketSendKey * s = new socketSendKey( this,NewKey,m_ui->textEditPassphraseToAdd->text().toAscii() ) ;
		s->sendKey();
	}

	QString a = QString( ZULUCRYPTzuluCrypt ) ;
	QString b = m_volumePath ;
	b.replace( "\"","\"\"\"" ) ;
	QString c = existingPassType ;
	QString d = ExistingKey ;
	QString e = newPassType ;
	QString f = NewKey ;

	QString exe = QString( "%1 -a -d \"%2\" %3 \"%4\" %5 \"%6\"" ).arg( a ).arg( b ).arg( c ).arg( d ).arg( e ).arg( f );

	m_isWindowClosable = false ;

	disableAll();

	runInThread * lakt = new runInThread( exe );

	connect( lakt,SIGNAL( finished( int ) ),this,SLOT( threadfinished( int ) ) ) ;
	lakt->start();
}

void luksaddkey::threadfinished( int status )
{
	m_isWindowClosable = true ;
	DialogMsg msg( this ) ;
	QStringList x ;
	QString success;
	switch( status ){
		case 0 :
			x = utility::luksEmptySlots( m_volumePath );
			if( x.isEmpty() == false )
				success = tr( "key added successfully.\n%1 / %2 slots are now in use" ).arg( x.at( 0 ) ).arg( x.at( 1 ) );
			else
				success = tr( "key added successfully." ) ;
			msg.ShowUIOK( tr( "SUCCESS!" ),success );
			return HideUI();
		case 1  : msg.ShowUIOK( tr( "ERROR!" ),tr( "presented key does not match any key in the volume" ) ) ;		      	break ;
		case 2  : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not open luks device" ) ) ;					     	break ;
		case 3  : msg.ShowUIOK( tr( "ERROR!" ),tr( "device is not a luks device" ) ) ;					     	break ;
		case 4  : msg.ShowUIOK( tr( "ERROR!" ),tr( "device does not exist" ) ) ;	                                      	break ;
		case 5  : msg.ShowUIOK( tr( "ERROR!" ),tr( "wrong arguments" ) ) ;							break ;
		case 6  : msg.ShowUIOK( tr( "ERROR!" ),tr( "one or more required argument( s ) for this operation is missing" ) );	break ;
		case 7  : msg.ShowUIOK( tr( "ERROR!" ),tr( "new passphrases do not match" ) );					   	break ;
		case 8  : msg.ShowUIOK( tr( "ERROR!" ),tr( "one or both keyfile( s ) does not exist" ) ) ;	                      	break ;
		case 9  : msg.ShowUIOK( tr( "ERROR!" ),tr( "couldnt get enought memory to hold the key file" ) ) ;			break ;
		case 10 : msg.ShowUIOK( tr( "ERROR!" ),tr( "all key slots are occupied, can not add any more keys" ) ) ;	      	break ;
		case 11 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to add a key to a system device,\nonly root user or members of group \"zulucrypt\" can do that\n") )	;break ;
		case 12 : msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open key file for reading" ) );			break ;
		case 13 : msg.ShowUIOK( tr( "ERROR!" ),tr( "only root user can add keys to system devices" ) );				break ;
		case 110: msg.ShowUIOK( tr( "ERROR!" ),tr( "can not find a partition that match presented UUID" ) );			break ;
		default : msg.ShowUIOK( tr( "ERROR!" ),tr( "unrecognized ERROR! with status number %1 encountered" ).arg( status ) );
	}
	enableAll();
}

void luksaddkey::disableAll()
{
	m_ui->labelExistingPassphrase->setEnabled( false );
	m_ui->labelLuksVolume->setEnabled( false );
	m_ui->labelNewPassphrase->setEnabled( false );
	m_ui->groupBox->setEnabled( false );
	m_ui->groupBox_2->setEnabled( false );
	m_ui->textEditExistingPassphrase->setEnabled( false );
	m_ui->textEditPassphraseToAdd->setEnabled( false );
	m_ui->textEditPathToVolume->setEnabled( false );
	m_ui->lineEditReEnterPassphrase->setEnabled( false );
	m_ui->labelNewPassphrase->setEnabled( false );
	m_ui->lineEditReEnterPassphrase->setEnabled( false );
	m_ui->pushButtonAdd->setEnabled( false );
	m_ui->pushButtonCancel->setEnabled( false );
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false );
	m_ui->pushButtonOpenFile->setEnabled( false );
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false );
	m_ui->pushButtonOpenPartition->setEnabled( false );
	m_ui->radioButtonNewPassphrase->setEnabled( false );
	m_ui->radioButtonNewPassphraseFromFile->setEnabled( false );
	m_ui->radioButtonPassphraseinVolume->setEnabled( false );
	m_ui->radioButtonPassphraseInVolumeFromFile->setEnabled( false );
	m_ui->labelReEnterPassphrase->setEnabled( false ) ;
}

void luksaddkey::enableAll()
{
	m_ui->labelExistingPassphrase->setEnabled( true );
	m_ui->labelLuksVolume->setEnabled( true );
	m_ui->labelNewPassphrase->setEnabled( true );
	m_ui->groupBox->setEnabled( true );
	m_ui->groupBox_2->setEnabled( true );
	m_ui->textEditExistingPassphrase->setEnabled( true );
	m_ui->textEditPassphraseToAdd->setEnabled( true );
	m_ui->textEditPathToVolume->setEnabled( true );
	m_ui->labelNewPassphrase->setEnabled( true );
	m_ui->pushButtonAdd->setEnabled( true );
	m_ui->pushButtonCancel->setEnabled( true );
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( true );
	m_ui->pushButtonOpenFile->setEnabled( true );
	m_ui->pushButtonOpenNewKeyFile->setEnabled( true );
	m_ui->pushButtonOpenPartition->setEnabled( true );
	m_ui->radioButtonNewPassphrase->setEnabled( true );
	m_ui->radioButtonNewPassphraseFromFile->setEnabled( true );
	m_ui->radioButtonPassphraseinVolume->setEnabled( true );
	m_ui->radioButtonPassphraseInVolumeFromFile->setEnabled( true );
	if( m_ui->radioButtonNewPassphrase->isChecked() == true )
		m_ui->labelReEnterPassphrase->setEnabled( true ) ;
	if( m_ui->radioButtonNewPassphraseFromFile->isChecked() == false )
		m_ui->lineEditReEnterPassphrase->setEnabled( true );
}

void luksaddkey::pbCancel( void )
{
	HideUI() ;
}

luksaddkey::~luksaddkey()
{
	m_keystrength->~keystrength() ;
	delete m_ui ;
}
