/*
 *
 *  Copyright ( c ) 2011
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
#include <QDebug>

#include <QCloseEvent>

#include "openvolume.h"
#include "task.h"
#include "dialogmsg.h"

luksaddkey::luksaddkey( QWidget * parent ) :
	QDialog( parent )
{
	m_ui = new Ui::luksaddkey() ;
	m_ui->setupUi( this ) ;

	m_isWindowClosable = true ;

	m_ui->textEditPathToVolume->setText( QString( "" ) ) ;

	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Password ) ;
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

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

	m_ui->lineEditReEnterPassphrase->setEchoMode( QLineEdit::Password ) ;

	m_ui->radioButtonNewPassphrase->setChecked( true ) ;
	m_ui->radioButtonPassphraseinVolume->setChecked( true ) ;

	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false ) ;

	m_ui->pushButtonOpenPartition->setIcon( QIcon( ":/partition.png" ) ) ;
	m_ui->pushButtonOpenFile->setIcon( QIcon( ":/file.png" ) ) ;

	this->installEventFilter( this ) ;
}

bool luksaddkey::eventFilter( QObject * watched,QEvent * event )
{
	if( utility::eventFilter( this,watched,event ) ){
		this->HideUI() ;
		return true ;
	}else{
		return false ;
	}
}

void luksaddkey::keyChanged( QString key )
{
	bool checkQuality = m_ui->radioButtonNewPassphrase->isChecked() && m_keystrength.canCheckQuality() ;
	if( checkQuality ){
		int st = m_keystrength.quality( key ) ;
		if( st < 0 ){
			this->setWindowTitle( tr( "Passphrase Quality: 0%" ) ) ;
		}else{
			this->setWindowTitle( tr( "Passphrase Quality: %1%" ).arg( QString::number( st ) ) ) ;
		}
	}else{
		this->setWindowTitle( tr( "Add A Key To A Volume" ) ) ;
	}
}

void luksaddkey::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	if( m_isWindowClosable ){
		this->HideUI() ;
	}
}

void luksaddkey::ShowUI( const QString& path )
{
	m_ui->textEditPathToVolume->setText( path ) ;
	this->ShowUI() ;
}

void luksaddkey::ShowUI()
{
	this->setFieldFocus() ;
	this->show() ;
}

void luksaddkey::setFieldFocus()
{
	if( m_ui->textEditPathToVolume->text().isEmpty() ){
		m_ui->textEditPathToVolume->setFocus() ;
	}else if( m_ui->textEditExistingPassphrase->text().isEmpty() ){
		m_ui->textEditExistingPassphrase->setFocus() ;
	}else if( m_ui->textEditPassphraseToAdd->text().isEmpty() ){
		m_ui->textEditPassphraseToAdd->setFocus() ;
	}else if( m_ui->radioButtonNewPassphrase->isChecked() ){
		if( m_ui->lineEditReEnterPassphrase->text().isEmpty() ){
			m_ui->lineEditReEnterPassphrase->setFocus() ;
		}else{
			m_ui->pushButtonAdd->setFocus() ;
		}
	}else{
		m_ui->pushButtonAdd->setFocus() ;
	}
}

void luksaddkey::HideUI()
{
	this->hide() ;
	emit HideUISignal() ;
}

void luksaddkey::pbOpenExisitingKeyFile( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "Existing Keyfile" ),utility::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->textEditExistingPassphrase->setText( Z ) ;
	}
	this->setFieldFocus() ;
}

void luksaddkey::pbOpenNewKeyFile( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "New Keyfile" ),utility::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->textEditPassphraseToAdd->setText( Z ) ;
	}
	this->setFieldFocus() ;
}

void luksaddkey::pbOpenFile( void )
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "Encrypted Volume Path" ),utility::homePath(),0 ) ;
	if( !Z.isEmpty() ){
		m_ui->textEditPathToVolume->setText( Z ) ;
	}
	this->setFieldFocus() ;
}

void luksaddkey::pbOpenPartition( void )
{
	openvolume * op = new openvolume( this ) ;
	op->showLuksOnly() ;
	connect( op,SIGNAL( clickedPartition( QString ) ),this,SLOT( ShowUI( QString ) ) ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	connect( op,SIGNAL( HideUISignal() ),this,SLOT( setFieldFocus() ) ) ;
	op->ShowAllPartitions() ;
}

void luksaddkey::rbExistingPassphrase( void )
{
	m_ui->textEditExistingPassphrase->setToolTip( tr( "Enter A Key" ) ) ;
	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Password ) ;
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false ) ;
	m_ui->labelExistingPassphrase->setText( tr( "Key" ) ) ;
	m_ui->textEditExistingPassphrase->clear() ;
	m_ui->pushButtonOpenExistingKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->textEditExistingPassphrase->setFocus() ;
}

void luksaddkey::rbExistingPassphraseFromFile( void )
{
	m_ui->textEditExistingPassphrase->setToolTip( tr( "Enter a path to a keyfile location" ) ) ;
	m_ui->textEditExistingPassphrase->setEchoMode( QLineEdit::Normal ) ;
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( true ) ;
	m_ui->labelExistingPassphrase->setText( tr( "Keyfile Path" ) ) ;
	m_ui->textEditExistingPassphrase->clear() ;
	m_ui->pushButtonOpenExistingKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->textEditExistingPassphrase->setFocus() ;
}

void luksaddkey::rbNewPassphrase( void )
{
	m_ui->textEditPassphraseToAdd->setToolTip( tr( "Enter a key" ) ) ;
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Password ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false ) ;
	m_ui->labelNewPassphrase->setText( tr( "Key" ) ) ;
	m_ui->textEditPassphraseToAdd->clear() ;
	m_ui->lineEditReEnterPassphrase->setEnabled( true ) ;
	m_ui->labelReEnterPassphrase->setEnabled( true ) ;
	m_ui->pushButtonOpenNewKeyFile->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->textEditPassphraseToAdd->setFocus() ;
}

void luksaddkey::rbNewPassphraseFromFile()
{
	m_ui->textEditPassphraseToAdd->setToolTip( tr( "Enter a path to a keyfile location" ) ) ;
	m_ui->textEditPassphraseToAdd->setEchoMode( QLineEdit::Normal ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( true ) ;
	m_ui->labelNewPassphrase->setText( tr( "Keyfile path" ) ) ;
	m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
	m_ui->lineEditReEnterPassphrase->clear() ;
	m_ui->labelReEnterPassphrase->setEnabled( false ) ;
	m_ui->pushButtonOpenNewKeyFile->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->textEditPassphraseToAdd->setFocus() ;
}

void luksaddkey::pbAdd( void )
{
	DialogMsg msg( this ) ;
	QString ExistingKey = m_ui->textEditExistingPassphrase->text() ;

	QString NewKey = m_ui->textEditPassphraseToAdd->text() ;
	QString NewKey_1 = m_ui->lineEditReEnterPassphrase->text() ;

	m_volumePath = m_ui->textEditPathToVolume->text() ;

	if( m_volumePath.isEmpty() ){
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
	}

	m_volumePath = utility::resolvePath( m_volumePath ) ;

	if( m_ui->radioButtonPassphraseInVolumeFromFile->isChecked() ){
		if( ExistingKey.isEmpty() ){
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}
	}

	if( m_ui->radioButtonNewPassphraseFromFile->isChecked() ){
		if( NewKey.isEmpty() ){
			return msg.ShowUIOK( tr( "ERROR!" ),tr( "Atleast one required field is empty" ) ) ;
		}
	}else{
		if( NewKey != NewKey_1 ){
			msg.ShowUIOK( tr( "ERROR!" ),tr( "Keys do not match" ) ) ;
			m_ui->textEditPassphraseToAdd->clear() ;
			m_ui->lineEditReEnterPassphrase->clear() ;
			m_ui->textEditPassphraseToAdd->setFocus() ;
			return ;
		}
	}

	QString existingPassType ;

	if( m_ui->radioButtonPassphraseInVolumeFromFile->isChecked() ){
		ExistingKey = utility::resolvePath( ExistingKey ).replace( "\"","\"\"\"" ) ;
		existingPassType = "-u" ;
	}else{
		existingPassType = "-u" ;
		ExistingKey = utility::keyPath() + "-existingKey" ;

		QString k = m_ui->textEditExistingPassphrase->text() ;

		utility::keySend( ExistingKey,k ) ;
	}

	QString newPassType ;
	if( m_ui->radioButtonNewPassphraseFromFile->isChecked() ){
		NewKey = utility::resolvePath( NewKey ).replace( "\"","\"\"\"" ) ;
		newPassType = "-n" ;
	}else{
		newPassType = "-n" ;

		NewKey = utility::keyPath() + "-newKey" ;

		QString k = m_ui->textEditPassphraseToAdd->text() ;

		utility::keySend( NewKey,k ) ;
	}

	const QString& a = QString( ZULUCRYPTzuluCrypt ) ;
	QString b = m_volumePath ;
	b.replace( "\"","\"\"\"" ) ;
	const QString& c = existingPassType ;
	const QString& d = ExistingKey ;
	const QString& e = newPassType ;
	const QString& f = NewKey ;

	auto r = "%1 -a -d \"%2\" %3 \"%4\" %5 \"%6\"" ;

	QString exe = utility::appendUserUID( r ).arg( a,b,c,d,e,f ) ;

	m_isWindowClosable = false ;

	this->disableAll() ;

	this->taskFinished( utility::exec( exe ).await() ) ;
}

void luksaddkey::keyAdded()
{
	m_volumePath.replace( "\"","\"\"\"" ) ;

	auto l = utility::luksEmptySlots( m_volumePath ).await() ;

	QString success ;

	if( l.isEmpty() ){

		success = tr( "Key added successfully." ) ;
	}else{
		QString x = tr( "Key added successfully.\n%1 / %2 slots are now in use" ) ;
		success = x.arg( l.first() ).arg( l.at( 1 ) ) ;
	}

	DialogMsg msg( this ) ;
	msg.ShowUIOK( tr( "SUCCESS!" ),success ) ;

	this->HideUI() ;
}

void luksaddkey::taskFinished( int r )
{
	m_isWindowClosable = true ;
	DialogMsg msg( this ) ;
	switch( r ){
		case 0  : return this->keyAdded() ;
		case 1  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Presented key does not match any key in the volume" ) ) ;		      	break ;
		case 2  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open luks volume" ) ) ;					     	break ;
		case 3  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Volume is not a luks volume" ) ) ;					     	break ;
		case 4  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to add a key to a system device,\nonly root user or members of group \"zulucrypt\" can do that\n" ) )	;break ;
		case 5  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not open volume in write mode" ) ) ;					break ;
		case 6  : msg.ShowUIOK( tr( "ERROR!" ),tr( "All key slots are occupied, can not add any more keys" ) ) ;		break ;
		case 7  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Can not get passphrase in silent mode" ) ) ;			   	break ;
		case 8  : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient memory to hold passphrase" ) ) ;	                      	break ;
		case 9  : msg.ShowUIOK( tr( "ERROR!" ),tr( "New passphrases do not match" ) ) ;						break ;
		case 10 : msg.ShowUIOK( tr( "ERROR!" ),tr( "One or more required argument(s) for this operation is missing" ) ) ;      	break ;
		case 11 : msg.ShowUIOK( tr( "ERROR!" ),tr( "One or both keyfile(s) does not exist" ) ) ;				break ;
		case 12 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insufficient privilege to open key file for reading" ) ) ;			break ;
		case 13 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Couldnt get enought memory to hold the key file" ) ) ;			break ;
		case 14 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a key from a socket" ) ) ;					break ;
		case 15 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not get elevated privilege,check binary permissions" ) ) ;		break ;
		case 110: msg.ShowUIOK( tr( "ERROR!" ),tr( "Can not find a partition that match presented UUID" ) ) ;			break ;
		case 113: msg.ShowUIOK( tr( "ERROR!" ),tr( "Device is not a luks device" ) ) ;						break ;
		default : msg.ShowUIOK( tr( "ERROR!" ),tr( "Unrecognized ERROR! with status number %1 encountered" ).arg( r ) ) ;
	}

	this->enableAll() ;
}

void luksaddkey::disableAll()
{
	m_ui->labelExistingPassphrase->setEnabled( false ) ;
	m_ui->labelLuksVolume->setEnabled( false ) ;
	m_ui->labelNewPassphrase->setEnabled( false ) ;
	m_ui->groupBox->setEnabled( false ) ;
	m_ui->groupBox_2->setEnabled( false ) ;
	m_ui->textEditExistingPassphrase->setEnabled( false ) ;
	m_ui->textEditPassphraseToAdd->setEnabled( false ) ;
	m_ui->textEditPathToVolume->setEnabled( false ) ;
	m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
	m_ui->labelNewPassphrase->setEnabled( false ) ;
	m_ui->lineEditReEnterPassphrase->setEnabled( false ) ;
	m_ui->pushButtonAdd->setEnabled( false ) ;
	m_ui->pushButtonCancel->setEnabled( false ) ;
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenFile->setEnabled( false ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( false ) ;
	m_ui->pushButtonOpenPartition->setEnabled( false ) ;
	m_ui->radioButtonNewPassphrase->setEnabled( false ) ;
	m_ui->radioButtonNewPassphraseFromFile->setEnabled( false ) ;
	m_ui->radioButtonPassphraseinVolume->setEnabled( false ) ;
	m_ui->radioButtonPassphraseInVolumeFromFile->setEnabled( false ) ;
	m_ui->labelReEnterPassphrase->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
}

void luksaddkey::enableAll()
{
	m_ui->labelExistingPassphrase->setEnabled( true ) ;
	m_ui->labelLuksVolume->setEnabled( true ) ;
	m_ui->labelNewPassphrase->setEnabled( true ) ;
	m_ui->groupBox->setEnabled( true ) ;
	m_ui->groupBox_2->setEnabled( true ) ;
	m_ui->textEditExistingPassphrase->setEnabled( true ) ;
	m_ui->textEditPassphraseToAdd->setEnabled( true ) ;
	m_ui->textEditPathToVolume->setEnabled( true ) ;
	m_ui->labelNewPassphrase->setEnabled( true ) ;
	m_ui->pushButtonAdd->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonOpenExistingKeyFile->setEnabled( true ) ;
	m_ui->pushButtonOpenFile->setEnabled( true ) ;
	m_ui->pushButtonOpenNewKeyFile->setEnabled( true ) ;
	m_ui->pushButtonOpenPartition->setEnabled( true ) ;
	m_ui->radioButtonNewPassphrase->setEnabled( true ) ;
	m_ui->radioButtonNewPassphraseFromFile->setEnabled( true ) ;
	m_ui->radioButtonPassphraseinVolume->setEnabled( true ) ;
	m_ui->radioButtonPassphraseInVolumeFromFile->setEnabled( true ) ;
	if( m_ui->radioButtonNewPassphrase->isChecked() ){
		m_ui->labelReEnterPassphrase->setEnabled( true ) ;
	}
	if( !m_ui->radioButtonNewPassphraseFromFile->isChecked() ){
		m_ui->lineEditReEnterPassphrase->setEnabled( true ) ;
	}
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
}

void luksaddkey::pbCancel( void )
{
	this->HideUI() ;
}

luksaddkey::~luksaddkey()
{
	delete m_ui ;
}
