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
#include "miscfunctions.h"
#include "createpartition.h"
#include "ui_createpartition.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QAbstractButton>
#include <QProcess>
#include <QThread>

#include <QDebug>
#include "../zuluCrypt-cli/constants.h"

createpartition::createpartition( QWidget * parent ) :
    QWidget( parent ),
    m_ui( new Ui::createpartition )
{
	m_ui->setupUi( this );
	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->lineEditVolumePath->setEnabled( false );
	m_ui->lineEditPassphrase1->setFocus();

	m_isWindowClosable = true ;

	m_keyStrength = new keystrength() ;

	connect( m_ui->pbOpenKeyFile,SIGNAL( clicked() ),this,SLOT( pbOpenKeyFile() ) );
	connect( m_ui->pbCreate,SIGNAL( clicked() ),this,SLOT( pbCreateClicked() ) );
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancelClicked() ) );
	connect( m_ui->rbPassphrase,SIGNAL( clicked() ),this,SLOT( rbPassphraseClicked() ) );
	connect( m_ui->rbPassphraseFromFile,SIGNAL( clicked() ),this,SLOT( rbPasssphraseFromFileClicked() ) );
	connect( m_ui->comboBoxVolumeType,SIGNAL( currentIndexChanged( int ) ),this,SLOT( rng( int ) ) ) ;
	connect( m_ui->lineEditPassphrase1,SIGNAL( textChanged( QString ) ),this,SLOT( keyChanged( QString ) ) ) ;
}

void createpartition::keyChanged( QString key )
{
	if( m_ui->rbPassphrase->isChecked() && m_keyStrength->canCheckQuality() ){

		if( key.length() <= 8 ){
			this->setWindowTitle( QString( "passphrase quality: BAD" ) ) ;
		}else{
			if( m_keyStrength->quality( key ) < 50 )
				this->setWindowTitle( QString( "passphrase quality: BAD" ) ) ;
			else
				this->setWindowTitle( QString( "passphrase quality: GOOD" ) ) ;
		}
	}else{
		this->setWindowTitle( QString( "create a new volume" ) ) ;
	}
}

void createpartition::findInstalledFs()
{
	DialogMsg msg( this );

	QStringList mkfsList =  QDir( QString( ZULUCRYPTmkfs_dir ) ).entryList().filter( "mkfs." ) ;

	if( mkfsList.size() == 0 ){
		disableAll();
		QString x = tr( "this tool expects to find file system creation tools at \"%1/\" " ).arg( ZULUCRYPTmkfs_dir );
		x += tr( "and it can not find them.\nIt is therefore not possible to create volumes using this tool." );
		msg.ShowUIOK( tr( "ERROR!" ),x ) ;
		return ;
	}

	int index = mkfsList.indexOf( QString( "mkfs.ext2" ) ) ;
	if( index != -1 )
		mkfsList.move( index,0 );
	index = mkfsList.indexOf( QString( "mkfs.ntfs" ) ) ;
	if( index != -1 )
		mkfsList.move( index,0 );
	index = mkfsList.indexOf( QString( "mkfs.vfat" ) ) ;
	if( index != -1 )
		mkfsList.move( index,0 );
	index = mkfsList.indexOf( QString( "mkfs.ext3" ) ) ;
	if( index != -1 )
		mkfsList.move( index,0 );
	index = mkfsList.indexOf( QString( "mkfs.ext4" ) ) ;
	if( index != -1 )
		mkfsList.move( index,0 );

	int j = mkfsList.size() ;
	QStringList mkfs ;
	QString entry ;
	for( int i = 0 ; i < j ; i++ ) {
		entry = mkfsList.at( i ) ;
		mkfs.append( entry.mid( entry.indexOf( "." ) + 1 ) );
	}
	m_ui->comboBoxFS->addItems( mkfs );
}

void createpartition::rng( int s )
{
	if( s == 1 )
		m_ui->comboBoxRNG->setEnabled( false );
	else
		m_ui->comboBoxRNG->setEnabled( true );
}

void createpartition::closeEvent( QCloseEvent *e )
{
	e->ignore();
	if( m_isWindowClosable == true )
		pbCancelClicked() ;
}

void createpartition::ShowPartition( QString volume )
{
	ShowUI( tr( "path to device" ),volume );
}

void createpartition::ShowFile( QString volume )
{
	ShowUI( tr( "path to file" ),volume );
}

void createpartition::eraseDataPartition()
{
	QString path = m_ui->lineEditVolumePath->text() ;

	if( path.left( 5 ) != QString( "/dev/" ) )
		return ;

	CreateVolumeDialog * cpd = new CreateVolumeDialog( path,this ) ;
	connect( cpd,SIGNAL( dialogResult( int ) ),this,SLOT( dialogResult( int ) ) );
	cpd->ShowUI() ;
}

void createpartition::dialogResult( int result )
{
	if( result == 0 )
		this->HideUI() ;
	else if( result == 1 )
		;
	else if( result == 2 ){
		erasedevice * ed = new erasedevice( this ) ;
		connect( ed,SIGNAL( HideUISignal() ),ed,SLOT( deleteLater() ) );
		ed->ShowUI( m_ui->lineEditVolumePath->text() );
	}
}

void createpartition::ShowUI( QString l,QString v )
{
	enableAll();
	m_ui->labelVolumePath->setText( l );
	m_ui->lineEditVolumePath->setText( v );
	m_ui->rbPassphrase->setChecked( true );
	m_ui->pbOpenKeyFile->setEnabled( false );
	m_ui->lineEditVolumePath->setEnabled( false );
	this->rbPassphraseClicked() ;
	m_ui->lineEditPassphrase1->setFocus();
	m_ui->comboBoxFS->setCurrentIndex( 0 );
	m_ui->comboBoxRNG->setCurrentIndex( 0 );
	m_ui->comboBoxVolumeType->setCurrentIndex( 0 );
	m_ui->labelRepeatPassPhrase->setEnabled( true );
	m_created = false ;
	this->show();
	this->eraseDataPartition();
	findInstalledFs() ;
}

void createpartition::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName( this,tr( "key file path" ),QDir::homePath(),0 );
	m_ui->lineEditPassphrase1->setText( Z );
}

void createpartition::pbCancelClicked()
{
	if( m_created == false ){
		QString s = m_ui->lineEditVolumePath->text() ;
		if( s.left( 5 ) != QString( "/dev/" ) )
			QFile::remove( s ) ;
	}
	HideUI() ;
}

void createpartition::HideUI()
{
	this->hide();
	emit HideUISignal();
}

void createpartition::enableAll()
{
	m_ui->labelPassPhrase->setEnabled( true );
	m_ui->labelVolumePath->setEnabled( true );
	m_ui->labelRepeatPassPhrase->setEnabled( true );
	m_ui->lineEditPassphrase1->setEnabled( true );
	m_ui->lineEditPassPhrase2->setEnabled( true );
	//m_ui->lineEditVolumePath->setEnabled( true );
	m_ui->pbCancel->setEnabled( true );
	m_ui->pbCreate->setEnabled( true );
	m_ui->pbOpenKeyFile->setEnabled( true );
	m_ui->labelfs->setEnabled( true );
	m_ui->labelvolumetype->setEnabled( true );
	m_ui->labelrng->setEnabled( true );
	m_ui->comboBoxFS->setEnabled( true );
	m_ui->comboBoxVolumeType->setEnabled( true );
	if( m_ui->comboBoxVolumeType->currentIndex() == 0 )
		m_ui->comboBoxRNG->setEnabled( true );
	m_ui->rbPassphrase->setEnabled( true );
	m_ui->rbPassphraseFromFile->setEnabled( true );
}

void createpartition::disableAll()
{
	m_ui->labelPassPhrase->setEnabled( false );
	m_ui->labelVolumePath->setEnabled( false );
	m_ui->labelRepeatPassPhrase->setEnabled( false );
	m_ui->lineEditPassphrase1->setEnabled( false );
	m_ui->lineEditPassPhrase2->setEnabled( false );
	m_ui->lineEditVolumePath->setEnabled( false );
	m_ui->pbCancel->setEnabled( false );
	m_ui->pbCreate->setEnabled( false );
	m_ui->pbOpenKeyFile->setEnabled( false );
	m_ui->labelfs->setEnabled( false );
	m_ui->labelrng->setEnabled( false );
	m_ui->labelvolumetype->setEnabled( false );
	m_ui->comboBoxFS->setEnabled( false );
	m_ui->comboBoxVolumeType->setEnabled( false );
	m_ui->comboBoxRNG->setEnabled( false );
	m_ui->rbPassphrase->setEnabled( false );
	m_ui->rbPassphraseFromFile->setEnabled( false );
}

void createpartition::rbPassphraseClicked()
{
	m_ui->pbOpenKeyFile->setEnabled( false );
	m_ui->lineEditPassPhrase2->setEnabled( true );
	m_ui->lineEditPassphrase1->clear();
	m_ui->lineEditPassPhrase2->clear();
	m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Password );
	m_ui->lineEditPassPhrase2->setEchoMode( QLineEdit::Password );
	m_ui->labelPassPhrase->setText( tr( "key" ) );
	m_ui->labelRepeatPassPhrase->setEnabled( true );
	m_ui->pbOpenKeyFile->setIcon( QIcon( QString( ":/passphrase.png" ) ) );
	this->setWindowTitle( QString( "create a new volume" ) ) ;
}

void createpartition::rbPasssphraseFromFileClicked()
{
	m_ui->pbOpenKeyFile->setEnabled( true );
	m_ui->lineEditPassphrase1->clear();
	m_ui->lineEditPassPhrase2->clear();
	m_ui->lineEditPassphrase1->setEchoMode( QLineEdit::Normal );
	m_ui->lineEditPassPhrase2->setEnabled( false );
	m_ui->labelPassPhrase->setText( tr( "keyfile" ) );
	m_ui->labelRepeatPassPhrase->setEnabled( false );
	m_ui->pbOpenKeyFile->setIcon( QIcon( QString( ":/keyfile.png" ) ) );
	this->setWindowTitle( QString( "create a new volume" ) ) ;
}

void createpartition::pbCreateClicked()
{
	DialogMsg msg( this ) ;

	QString volumePath   = m_ui->lineEditVolumePath->text() ;
	QString passphrase_1 = m_ui->lineEditPassphrase1->text() ;
	QString passphrase_2 = m_ui->lineEditPassPhrase2->text();

	if( volumePath.isEmpty() )
		return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "volume path field is empty" ) );

	QString source ;

	if ( m_ui->rbPassphraseFromFile->isChecked() == true ){
		if( passphrase_1.isEmpty() )
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) );

		source = QString( "-f" ) ;
	}else{
		if( passphrase_1.isEmpty() || passphrase_2.isEmpty() )
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "atleast one required field is empty" ) );
		if( passphrase_1 != passphrase_2 )
			return 	msg.ShowUIOK( tr( "ERROR!" ),tr( "passphrases do not match" ) );

		source = QString( "-p" ) ;
	}

	passphrase_1.replace( "\"","\"\"\"" ) ;
	volumePath.replace( "\"","\"\"\"" ) ;

	QString a = QString( ZULUCRYPTzuluCrypt ) ;
	QString b = volumePath ;
	QString c = m_ui->comboBoxFS->currentText() ;
	QString d = m_ui->comboBoxVolumeType->currentText() ;
	QString e = source ;
	QString f = passphrase_1 ;
	QString g = m_ui->comboBoxRNG->currentText();

	QString exe = QString( "%1 -c -k -d \"%2\" -z %3 -t %4 %5 \"%6\" -g %7" ).arg( a ).arg( b ).arg( c ).arg( d ).arg( e ).arg( f ).arg( g );

	m_isWindowClosable = false ;

	runInThread * cvt = new runInThread( exe ) ;

	connect( cvt,SIGNAL( finished( int ) ),this,SLOT( threadfinished( int ) ) ) ;
	disableAll();
	cvt->start() ;
}

void createpartition::threadfinished( int st )
{
	DialogMsg msg( this ) ;
	m_isWindowClosable = true ;
	QString x = tr( "volume created successfully" ) ;
	if( m_ui->comboBoxVolumeType->currentText() == QString( "luks" ) )
		x += tr( "\n\ncreating a backup of the luks header is strongly advised.\nPlease read documentation on why this is important." ) ;
	switch ( st ){
		case 0 : msg.ShowUIOK( tr( "SUCCESS!" ),x ) ;
		HideUI();													break  ;
		case 1 : msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to a file or device" ) );					break  ;
		case 2 : msg.ShowUIOK( tr( "ERROR!" ),tr( "wrong option type" ) );							break  ;
		case 3 : msg.ShowUIOK( tr( "ERROR!" ),tr( "could not create an encrypted volume in a file or device" ) );		break  ;
		case 4 : msg.ShowUIOK( tr( "ERROR!" ),tr( "one or more required argument( s ) for this operation is missing" ) );	break  ;
		case 5 : msg.ShowUIOK( tr( "ERROR!" ),tr( "wrong choice, exiting" ) );						break  ;
		case 6 : msg.ShowUIOK( tr( "ERROR!" ),tr( "couldnt get enought memory to hold the key file" ) ) ;			break  ;
		case 7 : msg.ShowUIOK( tr( "ERROR!" ),tr( "passphrases do not match" ) );						break  ;
		case 8 : msg.ShowUIOK( tr( "ERROR!" ),tr( "invalid path to key file" ) );						break  ;
		case 9 : msg.ShowUIOK( tr( "ERROR!" ),tr( "container file must be bigger than 3MB" ) ) ;				break  ;
		case 10: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to create a volume on a system partition.\
A system partition is a partition with an active entry in \"/etc/fstab\"\
and \"/etc/crypttab.\"\nRerun the tool from root's accout to proceed" ) ) ;
																break  ;
		case 11: msg.ShowUIOK( tr( "ERROR!" ),tr( "%1 not found" ).arg( ZULUCRYPTmkfs ) );					break  ;
		case 12: msg.ShowUIOK( tr( "ERROR!" ),tr( "user chose not to proceed" ) );						break  ;
		case 13: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to search for volume path" ) ) ;			break  ;
		case 14: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to create a volume in this device" ) ) ;		break  ;
		case 15: msg.ShowUIOK( tr( "ERROR!" ),tr( "insufficient privilege to open the file in write mode" ) ) ;		break  ;
		case 16: msg.ShowUIOK( tr( "ERROR!" ),tr( "there seem to be an opened mapper associated with the device" ) ) ;	break  ;
		case 17: msg.ShowUIOK( tr( "ERROR!" ),tr( "unable to resolve full path to device" ) ) ;				break  ;
		case 18: msg.ShowUIOK( tr( "ERROR!" ),tr( "can not create a volume on a mounted device" ) ) ;			break  ;

		case 110:msg.ShowUIOK( tr( "ERROR!" ),tr( "could not find any partition with the presented UUID" ) ) ;		break  ;
		default: msg.ShowUIOK( tr( "ERROR!" ),tr( "unrecognized ERROR! with status number %1 encountered" ).arg( st ) );
	}

	enableAll();
}

createpartition::~createpartition()
{
	m_keyStrength->~keystrength() ;
	delete m_ui;
}
