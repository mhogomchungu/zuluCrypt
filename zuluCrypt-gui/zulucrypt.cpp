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

#include "zulucrypt.h"
#include "ui_zulucrypt.h"
#include "utility.h"

#include <QProcess>
#include <QStringList>
#include <QMenu>
#include <QCursor>
#include <QByteArray>
#include <QColor>
#include <QBrush>
#include <iostream>
#include <QMessageBox>
#include <QFontDialog>
#include <QMetaType>
#include <QDebug>
#include <QKeySequence>
#include <QTranslator>

#include <QTableWidgetItem>
#include <QFont>
#include <QWidget>
#include <QList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QUrl>
#include <QMimeData>

#include "../zuluCrypt-cli/constants.h"
#include "../zuluMount-gui/oneinstance.h"
#include "password_dialog.h"
#include "openvolume.h"
#include "luksaddkey.h"
#include "luksdeletekey.h"
#include "createvolume.h"
#include "createfile.h"
#include "createkeyfile.h"
#include "favorites.h"
#include "cryptoinfo.h"
#include "erasedevice.h"
#include "managevolumeheader.h"
#include "cryptfiles.h"
#include "dialogmsg.h"
#include "managesystemvolumes.h"
#include "userfont.h"
#include "walletconfig.h"
#include "tablewidget.h"
#include "utility.h"
#include "task.h"
#include "lxqt_wallet/frontend/lxqt_wallet.h"

zuluCrypt::zuluCrypt( QWidget * parent ) :QMainWindow( parent ),m_trayIcon( 0 )
{
	this->setLocalizationLanguage() ;
	m_ui = new Ui::zuluCrypt ;

	QStringList l = QCoreApplication::arguments() ;

	m_device       = utility::cmdArgumentValue( l,"-d" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;
}

void zuluCrypt::setLocalizationLanguage()
{
	QTranslator * translator = new QTranslator( this ) ;
	QString program  = QString( "zuluCrypt-gui" ) ;
	QString lang     = utility::localizationLanguage( program ) ;
	QString langPath = utility::localizationLanguagePath( program ) ;

	QByteArray r = lang.toLatin1() ;

	QByteArray e( "en_US" ) ;
	if( e == r ){
		/*
		 *english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),langPath ) ;
		QCoreApplication::installTranslator( translator ) ;
	}
}

void zuluCrypt::setFolderOpener()
{
	//currently not used
	QStringList argv = QCoreApplication::arguments() ;
	if( argv.size() < 2 ){
		m_folderOpener = QString( "xdg-open" ) ;
	}else{
		m_folderOpener = argv.at( 1 ) ;
	}
}

void zuluCrypt::setUpApp()
{
	this->setupUIElements() ;
	this->setupConnections() ;
	this->initFont() ;
	this->initKeyCombo() ;
	this->initTray() ;
	this->updateVolumeList() ;
}

void zuluCrypt::updateVolumeList()
{
	m_ui->tableWidget->setEnabled( false ) ;
	Task * t = new Task() ;
	connect( t,SIGNAL( addItemToTable( QString,QString,QString ) ),this,SLOT( addItemToTable( QString,QString,QString ) ) ) ;
	connect( t,SIGNAL( finished( int ) ),this,SLOT( startUpdateFinished( int ) ) ) ;
	t->start( Task::updateVolumeList ) ;
}

void zuluCrypt::updateVolumeListAction()
{
	QTableWidget * table = m_ui->tableWidget ;
	if( table->isEnabled() ){
		while( table->rowCount() > 0 ){
			table->removeRow( 0 ) ;
		}
		this->updateVolumeList() ;
	}
}

void zuluCrypt::startUpdateFinished( int st )
{
	Q_UNUSED( st ) ;
	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;
	if( !m_device.isEmpty() ){
		QString y = m_device.split( "/" ).last() ;
		this->ShowPasswordDialog( m_device,y ) ;
	}
}

void zuluCrypt::processArgumentList()
{
}

void zuluCrypt::openVolumeFromArgumentList()
{
}

void zuluCrypt::initKeyCombo()
{
	QAction * rca = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Menu ) ;
	keys.append( Qt::CTRL + Qt::Key_M ) ;
	rca->setShortcuts( keys ) ;
	connect( rca,SIGNAL( triggered() ),this,SLOT( menuKeyPressed() ) ) ;
	this->addAction( rca ) ;
}

void zuluCrypt::initFont()
{
	userfont f( this ) ;
	setUserFont( f.getFont() ) ;
}

void zuluCrypt::raiseWindow()
{
	this->setVisible( true ) ;
	this->show() ; ;
	this->raise() ;
	this->setWindowState( Qt::WindowActive ) ;
}

void zuluCrypt::raiseWindow( QString device )
{
	this->setVisible( true ) ;
	this->show() ; ;
	this->raise() ;
	this->setWindowState( Qt::WindowActive ) ;
	QString y = device.split( "/" ).last() ;
	this->ShowPasswordDialog( device,y ) ;
}

void zuluCrypt::start()
{
	/*
	 * Entry point is here, the "instance" class checks if there is already a running zuluCrypt-gui process.
	 * If yes,this instance tells the existing instance to get focus and then it exits.
	 *
	 * If no,then it knowns it is the only running instance and it calls "setUpApp() to set up the GUI and
	 * runs.
	 */

	QString sockpath = QString( "zuluCrypt-gui.socket" ) ;
	oneinstance * instance = new oneinstance( this,sockpath,"raiseWindow",m_device ) ;
	connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
	connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;

	if( !instance->instanceExist() ){
		this->setUpApp() ;
	}
}

void zuluCrypt::initTray()
{
	QString home = QDir::homePath() + QString( "/.zuluCrypt/" ) ;
	QDir d( home ) ;
	if( !d.exists() ){
		d.mkdir( home ) ;
	}
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/tray" ) ) ;
	if( !f.exists() ){
		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write( "1" ) ;
		f.close() ;
	}

	f.open( QIODevice::ReadOnly ) ;
	QByteArray c = f.readAll() ;
	f.close() ;

	m_ui->actionTray_icon->setCheckable( true ) ;

	if( c.at( 0 ) == '1' ){
		m_ui->actionTray_icon->setChecked( true ) ;
		m_trayIcon->show() ;
	}else{
		m_ui->actionTray_icon->setChecked( false ) ;
		m_trayIcon->hide() ;
	}
}

void zuluCrypt::setupUIElements()
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowIcon( QIcon( QString( ":/zuluCrypt.png" ) ) ) ;

	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_trayIcon->setIcon( QIcon( QString( ":/zuluCrypt.png" ) ) ) ;

	QMenu * trayMenu = new QMenu( this ) ;

	trayMenu->addAction( tr( "show the interface" ),this,SLOT( raiseWindow() ) ) ;
	trayMenu->addAction( tr( "quit" ),this,SLOT( closeApplication() ) ) ;

	m_trayIcon->setContextMenu( trayMenu ) ;

	m_ui->tableWidget->setColumnWidth( 0,298 ) ;
	m_ui->tableWidget->setColumnWidth( 1,336 ) ;
	//m_ui->tableWidget->setColumnWidth( 2,70 ) ;
}

void zuluCrypt::itemEntered( QTableWidgetItem * item )
{
	int row = item->row() ;
	QTableWidget * table = item->tableWidget() ;
	QString m_point = table->item( row,1 )->text() ;
	if( !m_point.isEmpty() ){
		item->setToolTip( utility::shareMountPointToolTip( m_point ) ) ;
	}
}

void zuluCrypt::setupConnections()
{
	m_ui->tableWidget->setMouseTracking( true ) ;
	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionErase_data_on_device,SIGNAL( triggered() ),this,SLOT( ShowEraseDataDialog() ) ) ;
	connect( this,SIGNAL( favClickedVolume( QString,QString ) ),this,SLOT( ShowPasswordDialog( QString,QString ) ) ) ;
	connect( m_ui->actionPartitionOpen,SIGNAL( triggered() ),this,SLOT( ShowOpenPartition() ) ) ;
	connect( m_ui->actionFileOpen,SIGNAL( triggered() ),this,SLOT( ShowPasswordDialog() ) ) ;
	connect( m_ui->actionFileCreate,SIGNAL( triggered() ),this,SLOT( ShowCreateFile() ) ) ;
	connect( m_ui->actionManage_names,SIGNAL( triggered() ),this,SLOT( ShowFavoritesEntries() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionCreatekeyFile,SIGNAL( triggered() ),this,SLOT( ShowCreateKeyFile() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionAbout,SIGNAL( triggered() ),this,SLOT( aboutMenuOption() ) ) ;
	connect( m_ui->actionAddKey,SIGNAL( triggered() ),this,SLOT( ShowAddKey() ) ) ;
	connect( m_ui->actionDeleteKey,SIGNAL( triggered() ),this,SLOT( ShowDeleteKey() ) ) ;
	connect( m_ui->actionPartitionCreate,SIGNAL( triggered() ),this,SLOT( ShowNonSystemPartitions() ) ) ;
	connect( m_ui->actionInfo,SIGNAL( triggered() ),this,SLOT( info() ) ) ;
	connect( m_ui->actionFonts,SIGNAL( triggered() ),this,SLOT( fonts() ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( aboutToShow() ),this,SLOT( readFavorites() ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( aboutToHide() ),this,SLOT( favAboutToHide() ) ) ;
	connect( m_ui->actionTray_icon,SIGNAL( triggered() ),this,SLOT( trayProperty() ) ) ;
	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( trayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;
	connect( m_ui->menuFavorites,SIGNAL( triggered( QAction * ) ),this,SLOT( favClicked( QAction * ) ) ) ;
	connect( this,SIGNAL( luksAddKey( QString ) ),this,SLOT( ShowAddKeyContextMenu( QString ) ) ) ;
	connect( this,SIGNAL( luksDeleteKey( QString ) ),this,SLOT( ShowDeleteKeyContextMenu( QString ) ) ) ;
	connect( m_ui->action_close,SIGNAL( triggered() ),this,SLOT( closeApplication() ) ) ;
	connect( m_ui->action_update_volume_list,SIGNAL( triggered() ),this,SLOT( updateVolumeListAction() ) ) ;
	connect( m_ui->actionMinimize_to_tray,SIGNAL( triggered() ),this,SLOT( minimizeToTray() ) ) ;
	connect( m_ui->actionClose_all_opened_volumes,SIGNAL( triggered() ),this,SLOT( closeAllVolumes() ) ) ;
	connect( m_ui->actionPermission_problems,SIGNAL( triggered() ),this,SLOT( permissionExplanation() ) ) ;
	connect( m_ui->actionEncrypt_file,SIGNAL( triggered() ),this,SLOT( encryptFile() ) ) ;
	connect( m_ui->actionDecrypt_file,SIGNAL( triggered() ),this,SLOT( decryptFile() ) ) ;
	connect( m_ui->actionLuks_header_backup,SIGNAL( triggered() ),this,SLOT( HelpLuksHeaderBackUp() ) ) ;
	connect( m_ui->actionManage_system_partitions,SIGNAL( triggered() ),this,SLOT( ShowManageSystemPartitions() ) ) ;
	connect( m_ui->actionManage_non_system_partitions,SIGNAL( triggered() ),this,SLOT( ShowManageNonSystemPartitions() ) ) ;
	connect( m_ui->actionChange_internal_wallet_password,SIGNAL( triggered() ),this,SLOT( changePassWordOfInternalWallet() ) ) ;
	connect( m_ui->actionManage_volumes_in_kde_wallet,SIGNAL( triggered() ),this,SLOT( manageVolumesInKDEWallet() ) ) ;
	connect( m_ui->actionManage_volumes_in_gnome_wallet,SIGNAL( triggered() ),this,SLOT( manageVolumesInGNOMEWallet() ) ) ;
	connect( m_ui->actionManage_volumes_in_internal_wallet,SIGNAL( triggered() ),this,SLOT( manageVolumesInInternalWallet() ) ) ;

	m_ui->actionRestore_header->setText( tr( "restore volume header" ) ) ;
	m_ui->actionBackup_header->setText( tr( "backup volume header" ) ) ;

	connect( m_ui->actionBackup_header,SIGNAL( triggered() ),this,SLOT( volumeHeaderBackUp() ) ) ;
	connect( m_ui->actionRestore_header,SIGNAL( triggered() ),this,SLOT( volumeRestoreHeader() ) ) ;
	m_ui->actionManage_volumes_in_gnome_wallet->setEnabled( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ) ;
	m_ui->actionManage_volumes_in_kde_wallet->setEnabled( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ) ;

	connect( m_ui->menuOptions,SIGNAL( aboutToShow() ),this,SLOT( optionMenuAboutToShow() ) ) ;

	m_ui->actionManage_system_partitions->setEnabled( utility::userIsRoot() ) ;
	m_ui->actionManage_non_system_partitions->setEnabled( utility::userIsRoot() ) ;

	this->setAcceptDrops( true ) ;
}

void zuluCrypt::optionMenuAboutToShow()
{
	bool b = LxQt::Wallet::walletExists( LxQt::Wallet::internalBackEnd,utility::walletName(),utility::applicationName() ) ;
	m_ui->actionChange_internal_wallet_password->setEnabled( b ) ;
}

walletconfig * zuluCrypt::setUpWalletConfig()
{
	walletconfig * cfg = new walletconfig( this ) ;
	connect( cfg,SIGNAL( couldNotOpenWallet() ),this,SLOT( failedToOpenWallet() ) ) ;
	return cfg ;
}

void zuluCrypt::manageVolumesInGNOMEWallet()
{
	setUpWalletConfig()->ShowUI( LxQt::Wallet::secretServiceBackEnd ) ;
}

void zuluCrypt::manageVolumesInInternalWallet()
{
	setUpWalletConfig()->ShowUI( LxQt::Wallet::internalBackEnd ) ;
}

void zuluCrypt::manageVolumesInKDEWallet()
{
	setUpWalletConfig()->ShowUI( LxQt::Wallet::kwalletBackEnd ) ;
}

void zuluCrypt::failedToOpenWallet()
{
	//DialogMsg msg( this ) ;
	//msg.ShowUIOK( tr( "ERROR" ),tr( "could not open selected wallet" ) ) ;
}

void zuluCrypt::changePassWordOfInternalWallet()
{
	m_wallet = LxQt::Wallet::getWalletBackend() ;
	m_wallet->setInterfaceObject( this ) ;
	m_wallet->changeWalletPassWord( utility::walletName(),utility::applicationName() ) ;
}

void zuluCrypt::walletIsOpen( bool b )
{
	Q_UNUSED( b ) ;
}

void zuluCrypt::walletpassWordChanged( bool b )
{
	Q_UNUSED( b ) ;
	m_wallet->deleteLater() ;
}

void zuluCrypt::permissionExplanation()
{
	DialogMsg m( this ) ;
	m.ShowPermissionProblem( QString( "" ) ) ;
}

void zuluCrypt::ShowManageSystemPartitions()
{
	manageSystemVolumes * msv = new manageSystemVolumes( this ) ;
	connect( msv,SIGNAL( HideUISignal() ),msv,SLOT(deleteLater() ) ) ;

	QDir d ;
	QFile f ;

	/*
	 * move config files to /etc/zuluCrypt/
	 */
	d.mkdir( "/etc/zuluCrypt" ) ;
	f.rename( QString( "/etc/zuluCrypt-system" ),QString( "/etc/zuluCrypt/system_volumes.list" ) ) ;

	f.rename( QString( "/etc/zuluCrypt-nonsystem" ),QString( "/etc/zuluCrypt/nonsystem_volumes.list" ) ) ;

	msv->ShowUI( QString( "/etc/zuluCrypt/system_volumes.list" ) ) ;
}

void zuluCrypt::ShowManageNonSystemPartitions()
{
	manageSystemVolumes * msv = new manageSystemVolumes( this ) ;
	connect( msv,SIGNAL( HideUISignal() ),msv,SLOT(deleteLater() ) ) ;

	QDir d ;
	QFile f ;

	/*
	 * move config files to /etc/zuluCrypt/
	 */

	d.mkdir( "/etc/zuluCrypt" ) ;
	f.rename( QString( "/etc/zuluCrypt-system" ),QString( "/etc/zuluCrypt/system_volumes.list" ) ) ;

	f.rename( QString( "/etc/zuluCrypt-nonsystem" ),QString( "/etc/zuluCrypt/nonsystem_volumes.list" ) ) ;

	msv->ShowUI( QString( "/etc/zuluCrypt/nonsystem_volumes.list" ) ) ;
}

void zuluCrypt::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;

	int rowCount = m_ui->tableWidget->rowCount() ;

	if( rowCount > 12 ){
		m_ui->tableWidget->setColumnWidth( 2,70 ) ;
	}else{
		m_ui->tableWidget->setColumnWidth( 2,95 ) ;
	}
}

void zuluCrypt::closeAllVolumes()
{
	Task * t = new Task( m_ui->tableWidget ) ;
	connect( t,SIGNAL( taskResult( QTableWidgetItem *,int ) ),this,SLOT( closeAll( QTableWidgetItem *,int ) ) ) ;
	t->start( Task::closeAllVolumeTask ) ;
}

void zuluCrypt::closeAll( QTableWidgetItem * item,int st )
{
	if( st ){
		closeStatusErrorMessage( st ) ;
	}else{
		removeRowFromTable( item->row() ) ;
	}
}

void zuluCrypt::minimizeToTray()
{
	if( m_ui->actionTray_icon->isChecked() ){
		this->hide() ;
	}else{
		m_ui->actionTray_icon->setChecked( true ) ;
		trayProperty() ;
		this->hide() ;
	}
}

void zuluCrypt::closeEvent( QCloseEvent * e )
{
	if( m_trayIcon->isVisible() ){
		this->hide() ;
		e->ignore() ;
	}else{
		this->hide() ;
		e->accept() ;
	}
}

void zuluCrypt::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void zuluCrypt::dropEvent( QDropEvent * e )
{
	const QMimeData * m = e->mimeData() ;
	QList<QUrl> l = m->urls() ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){
		m_device = l.at( i ).path() ;
		if( utility::pathPointsToAFile( m_device ) ){
			QString y = m_device.split( "/" ).last() ;
			this->ShowPasswordDialog( m_device,y ) ;
		}
	}
}

void zuluCrypt::closeApplication()
{
	m_trayIcon->hide() ;
	this->hide() ;
	QCoreApplication::quit() ;
}

void zuluCrypt::trayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){
		if( this->isVisible() ){
			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void zuluCrypt::trayProperty()
{
	m_ui->actionTray_icon->setEnabled( false ) ;
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/tray" ) ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray c = f.readAll() ;
	f.close() ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	QByteArray data ;
	if( c.at( 0 ) == '1' ){
		data.append( '0' ) ;
		f.write( data ) ;
		m_trayIcon->hide() ;
	}else{
		data.append( '1' ) ;
		f.write( data ) ;
		m_trayIcon->show() ;
	}
	f.close() ;
	m_ui->actionTray_icon->setEnabled( true ) ;
}

void zuluCrypt::fonts()
{
	int size = 11 ;
	bool ok ;
	QFont Font = QFontDialog::getFont( &ok,this->font(),this ) ;
	if( ok == true ){
		int k = Font.pointSize() ;
		if( k > size ){
			k = size ;
			Font.setPointSize( k ) ;
			UIMessage( tr( "info" ),tr( "resetting font size to %1 because larger font sizes do not fit" ).arg( QString::number( size ) ) ) ;
		}

		this->setUserFont( Font ) ;
		userfont f( this ) ;
		f.saveFont( Font ) ;
	}
}

void zuluCrypt::setUserFont( QFont Font )
{
	this->setFont( Font ) ;

	m_ui->tableWidget->horizontalHeaderItem( 0 )->setFont( Font ) ;
	m_ui->tableWidget->horizontalHeaderItem( 1 )->setFont( Font ) ;
	m_ui->tableWidget->horizontalHeaderItem( 2 )->setFont( Font ) ;

	m_ui->actionAbout->setFont( Font ) ;
	m_ui->actionAddKey->setFont( Font ) ;
	m_ui->actionCreatekeyFile->setFont( Font ) ;
	m_ui->actionDeleteKey->setFont( Font ) ;
	m_ui->actionFavorite_volumes->setFont( Font ) ;
	m_ui->actionFileCreate->setFont( Font ) ;
	m_ui->actionFileOpen->setFont( Font ) ;
	m_ui->actionFonts->setFont( Font ) ;
	m_ui->actionInfo->setFont( Font ) ;
	m_ui->actionManage_favorites->setFont( Font ) ;
	m_ui->actionPartitionCreate->setFont( Font ) ;
	m_ui->actionPartitionOpen->setFont( Font ) ;
	m_ui->actionSelect_random_number_generator->setFont( Font ) ;
	m_ui->actionTray_icon->setFont( Font ) ;
	m_ui->menuFavorites->setFont( Font ) ;
	m_ui->actionManage_names->setFont( Font ) ;
	m_ui->actionBackup_header->setFont( Font ) ;
	m_ui->actionRestore_header->setFont( Font ) ;
	m_ui->actionEncrypt_file->setFont( Font ) ;
	m_ui->actionDecrypt_file->setFont( Font ) ;
	m_ui->menu_zc->setFont( Font ) ;
	m_ui->actionPermission_problems->setFont( Font ) ;
	m_ui->actionLuks_header_backup->setFont( Font ) ;
	m_ui->actionManage_system_partitions->setFont( Font ) ;
	m_ui->actionManage_non_system_partitions->setFont( Font ) ;
	m_ui->actionManage_volumes_in_gnome_wallet->setFont( Font ) ;
	m_ui->actionManage_volumes_in_internal_wallet->setFont( Font ) ;
	m_ui->actionManage_volumes_in_kde_wallet->setFont( Font ) ;
	m_ui->actionUse_kde_default_wallet->setFont( Font ) ;
	m_ui->actionChange_internal_wallet_password->setFont( Font ) ;
}

void zuluCrypt::info()
{
	cryptoinfo * cinfo = new cryptoinfo( this ) ;
	connect( cinfo,SIGNAL( closeUISignal() ),cinfo,SLOT( deleteLater() ) ) ;
	cinfo->show() ;
}

void zuluCrypt::createEncryptedpartitionUI()
{
	emit SignalShowNonSystemPartitions() ;
}

void zuluCrypt::aboutMenuOption( void )
{
	QString license = QString( "%1\n\n\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 2 of the License, or \
( at your option ) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>." ).arg( VERSION_STRING ) ;

	DialogMsg m( this ) ;
	m.ShowUIInfo( tr( "about zuluCrypt" ),license ) ;
}

void zuluCrypt::HelpLuksHeaderBackUp()
{
	QString msg = tr( "\nAll luks based encrypted volumes have what is called a \"luks header\".\n\n\
A luks header is responsible for storing information necessary to open a luks based volume and any damage \
to it will makes it impossible to open the volume causing permanent loss of encrypted data.\n\n\
The damage to the header is usually caused by accidental formatting of the device or use of \
some buggy partitioning tools or wrongly reassembled logical volumes.\n\n\
Having a backup of the luks header is strongly advised because it is the only way the encrypted data will be accessible \
again after the header is restored if the header on the volume get corrupted.\n\n\
For more information, please read the FAQ at: http://code.google.com/p/cryptsetup/wiki/FrequentlyAskedQuestions" ) ;

	DialogMsg m( this ) ;
	m.ShowUIInfo( tr( "important information on luks header" ),msg ) ;
}

void zuluCrypt::addItemToTable( QString device,QString m_point,QString type )
{
	QStringList s ;

	QString dev = device ;
	dev.replace( "\"","\"\"\"" ) ;

	s.append( device ) ;
	s.append( m_point ) ;
	if( type == QString( "crypto_LUKS" ) ){
		s.append( QString( "luks" ) ) ;
	}else if( type == QString( "crypto_PLAIN" ) ){
		s.append( QString( "plain" ) ) ;
	}else if( type == QString( "crypto_TCRYPT" ) ){
		s.append( QString( "tcrypt" ) ) ;
	}
	tablewidget::addRowToTable( m_ui->tableWidget,s ) ;
}

void zuluCrypt::removeRowFromTable( int x )
{
	tablewidget::deleteRowFromTable( m_ui->tableWidget,x ) ;
}

void zuluCrypt::volume_property()
{
	m_ui->tableWidget->setEnabled( false ) ;

	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString x = m_ui->tableWidget->item( item->row(),0 )->text() ;
	QString y = m_ui->tableWidget->item( item->row(),1 )->text() ;

	Task * t = new Task( x,y ) ;
	connect( t,SIGNAL( finished( QString ) ),this,SLOT( volumePropertyTaskFinished( QString ) ) ) ;
	t->start( Task::volumePropertiesTask ) ;
}

void zuluCrypt::volumePropertyTaskFinished( QString properties )
{
	DialogMsg msg( this ) ;
	if( properties.isEmpty() ){
		msg.ShowUIOK( tr( "ERROR!"),tr( "volume is not open or was opened by a different user" ) ) ;
	}else{
		msg.ShowUIVolumeProperties( tr( "volume properties" ),properties ) ;
	}
	m_ui->tableWidget->setEnabled( true ) ;
}

void zuluCrypt::favAboutToHide()
{
}

void zuluCrypt::favClicked( QAction * e )
{
	QStringList l = e->text().split( "\t" ) ;
	emit favClickedVolume( l.at( 0 ),l.at( 1 ) ) ;
}

void zuluCrypt::readFavorites()
{
	QAction * ac ;
	m_ui->menuFavorites->clear() ;
	QStringList l = utility::readFavorites() ;
	if( !l.isEmpty() ){
		for( int i = 0 ; i < l.size() - 1 ; i++ ){
			ac = new QAction( l.at( i ),m_ui->menuFavorites ) ;
			m_ui->menuFavorites->addAction( ac ) ;
		}
	}else{
		ac = new QAction( tr( "list is empty" ),m_ui->menuFavorites ) ;
		ac->setEnabled( false ) ;
		m_ui->menuFavorites->addAction( ac ) ;
	}
}

void zuluCrypt::addToFavorite()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString x = m_ui->tableWidget->item( item->row(),0 )->text() ;
	QString y = x.split( "/" ).last() ;
	utility::addToFavorite( x,y ) ;
}

void zuluCrypt::menuKeyPressed()
{
	QTableWidgetItem * it = m_ui->tableWidget->currentItem() ;
	itemClicked( it,false ) ;
}

void zuluCrypt::fileManagerOpenStatus( int exitCode,int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ) ;
	}
}

void zuluCrypt::openFolder()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString path = m_ui->tableWidget->item( item->row(),1 )->text() ;

	Task * t = new Task( m_folderOpener,path ) ;
	connect( t,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	t->start( Task::openMountPoint ) ;
}

void zuluCrypt::itemClicked( QTableWidgetItem * it )
{
	itemClicked( it,true ) ;
}

void zuluCrypt::itemClicked( QTableWidgetItem * item,bool clicked )
{
	QMenu m ;
	m.setFont( this->font() ) ;
	connect( m.addAction( tr( "close" ) ),SIGNAL( triggered() ),this,SLOT( close() ) ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "open folder" ) ) ,SIGNAL( triggered() ),this,SLOT( openFolder() ) ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "properties" ) ) ,SIGNAL( triggered() ),this,SLOT( volume_property() ) ) ;

	m.addSeparator() ;

	if( m_ui->tableWidget->item( item->row(),2 )->text() == QString( "luks" ) ){
		m.addSeparator() ;
		connect( m.addAction( tr( "add key" ) ),SIGNAL( triggered() ),this,SLOT( luksAddKeyContextMenu() ) ) ;
		connect( m.addAction( tr( "remove key" ) ),SIGNAL( triggered() ),this,SLOT( luksDeleteKeyContextMenu() ) ) ;
		m.addSeparator() ;
		connect( m.addAction( tr( "backup luks header" ) ),SIGNAL( triggered() ),this,SLOT( luksHeaderBackUpContextMenu() ) ) ;
	}

	m.addSeparator() ;

	QString volume_id = m_ui->tableWidget->item( item->row(),0 )->text() + QString( "\t" ) ;

	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;

	f.open( QIODevice::ReadOnly ) ;

	QByteArray data = f.readAll() ;

	QAction * ac = m.addAction( tr( "add to favorite" ) ) ;

	if( data.contains( volume_id.toLatin1() ) ){
		ac->setEnabled( false ) ;
	}else{
		ac->setEnabled( true ) ;
		ac->connect( ac,SIGNAL( triggered() ),this,SLOT( addToFavorite() ) ) ;
	}
	if( clicked ){
		m.exec( QCursor::pos() ) ;
	}else{
		int x = m_ui->tableWidget->columnWidth( 0 ) ;
		int y = m_ui->tableWidget->rowHeight( item->row() ) * item->row() + 20 ;

		m.addSeparator() ;
		m.addAction( tr( "cancel" ) ) ;
		m.exec( m_ui->tableWidget->mapToGlobal( QPoint( x,y ) ) ) ;
	}
}

void zuluCrypt::setDefaultWallet()
{
	m_ui->actionUse_kde_default_wallet->setEnabled( false ) ;
	m_ui->actionUse_kde_default_wallet->setEnabled( true ) ;
}

void zuluCrypt::luksAddKeyContextMenu( void )
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	emit luksAddKey( m_ui->tableWidget->item( item->row(),0 )->text() ) ;
}

void zuluCrypt::luksDeleteKeyContextMenu( void )
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	emit luksDeleteKey( m_ui->tableWidget->item( item->row(),0 )->text() ) ;
}

void zuluCrypt::UIMessage( QString title,QString message )
{
	DialogMsg msg( this ) ;
	msg.ShowUIOK( title,message ) ;
}

void zuluCrypt::closeStatus( int st )
{
	m_ui->tableWidget->setEnabled( true ) ;
	if( st == 0 ){
		removeRowFromTable( m_ui->tableWidget->currentItem()->row() ) ;
	}else{
		closeStatusErrorMessage( st ) ;
	}
}

void zuluCrypt::closeStatusErrorMessage( int st )
{
	switch ( st ) {
		case 1 :UIMessage( tr( "ERROR!" ),tr( "close failed, volume is not open or was opened by a different user" ) ) ;			break ;
		case 2 :UIMessage( tr( "ERROR!" ),tr( "close failed, one or more files in the volume are in use." ) ) ;					break ;
		case 3 :UIMessage( tr( "ERROR!" ),tr( "close failed, volume does not have an entry in /etc/mtab" ) ) ;					break ;
		case 4 :UIMessage( tr( "ERROR!" ),tr( "close failed, could not get a lock on /etc/mtab~" ) ) ;						break ;
		case 5 :UIMessage( tr( "ERROR!" ),tr( "close failed, volume is unmounted but could not close mapper,advice to close it manually" ) ) ;	break ;
		case 6 :UIMessage( tr( "ERROR!" ),tr( "close failed, could not resolve full path of device\n" ) ) ;               			break ;
		case 7 :UIMessage( tr( "ERROR!" ),tr( "close failed, shared mount point appear to be busy\n" ) ) ;					break ;
		case 8 :UIMessage( tr( "ERROR!" ),tr( "close failed, shared mount point appear to belong to a different user\n" ) ) ;			break ;
		case 9 :UIMessage( tr( "ERROR!" ),tr( "close failed, shared mount point appear to be in an ambiguous state,advice to unmount manually" )) ;break ;
		case 110:UIMessage( tr( "ERROR!" ),tr( "close failed, could not find any partition with the presented UUID" ) ) ;			break ;
		default:UIMessage( tr( "ERROR!" ),tr( "unrecognized error with status number %1 encountered" ).arg( st ) ) ;
	}
}

void zuluCrypt::close()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString vol = m_ui->tableWidget->item( item->row(),0 )->text().replace( "\"","\"\"\"" ) ;
	QString exe = QString( ZULUCRYPTzuluCrypt ) + QString( " -q -d " ) + QString( "\"" ) + vol + QString( "\"" ) ;
	m_ui->tableWidget->setEnabled( false ) ;

	Task * t = new Task( exe ) ;
	connect( t,SIGNAL( finished( int ) ),this,SLOT( closeStatus( int ) ) ) ;
	t->start( Task::closeVolumeTask ) ;
}

managevolumeheader * zuluCrypt::setUpManageVolumeHeader()
{
	managevolumeheader * bkh = new managevolumeheader( this ) ;
	connect( bkh,SIGNAL( HideUISignal() ),bkh,SLOT( deleteLater() ) ) ;
	return bkh ;
}

void zuluCrypt::volumeRestoreHeader()
{
	this->setUpManageVolumeHeader()->restoreHeader() ;
}

void zuluCrypt::volumeHeaderBackUp()
{
	this->setUpManageVolumeHeader()->backUpHeader() ;
}

void zuluCrypt::luksHeaderBackUpContextMenu()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString device = m_ui->tableWidget->item( item->row(),0 )->text() ;
	this->setUpManageVolumeHeader()->backUpHeader( device ) ;
}

luksaddkey * zuluCrypt::setUpluksaddkey()
{
	luksaddkey * addKey = new luksaddkey( this ) ;
	connect( addKey,SIGNAL( HideUISignal() ),addKey,SLOT( deleteLater() ) ) ;
	return addKey ;
}

void zuluCrypt::ShowAddKeyContextMenu( QString key )
{
	setUpluksaddkey()->ShowUI( key ) ;
}

void zuluCrypt::ShowAddKey()
{
	setUpluksaddkey()->ShowUI() ;
}

luksdeletekey * zuluCrypt::setUpluksdeletekey()
{
	luksdeletekey * deleteKey = new luksdeletekey( this ) ;
	connect( deleteKey,SIGNAL( HideUISignal() ),deleteKey,SLOT( deleteLater() ) ) ;
	return deleteKey ;
}

void zuluCrypt::ShowDeleteKeyContextMenu( QString key )
{
	setUpluksdeletekey()->ShowUI( key ) ;
}

void zuluCrypt::ShowDeleteKey()
{
	setUpluksdeletekey()->ShowUI() ;
}

void zuluCrypt::ShowCreateKeyFile()
{
	createkeyfile * ckf = new createkeyfile( this ) ;
	connect( ckf,SIGNAL( HideUISignal() ),ckf,SLOT( deleteLater() ) ) ;
	ckf->ShowUI() ;
}

void zuluCrypt::ShowFavoritesEntries()
{
	favorites * f = new favorites( this ) ;
	connect( f,SIGNAL( HideUISignal() ),f,SLOT( deleteLater() ) ) ;
	f->ShowUI() ;
}

void zuluCrypt::ShowCreateFile()
{
	createfile * cf = new createfile( this ) ;
	connect( cf,SIGNAL( HideUISignal() ),cf,SLOT( deleteLater() ) ) ;
	connect( cf,SIGNAL( fileCreated( QString ) ),this,SLOT( FileCreated( QString ) ) ) ;
	cf->showUI() ;
}

createvolume * zuluCrypt::setUpCreatepartition()
{
	createvolume * cp = new createvolume( this ) ;
	connect( cp,SIGNAL( HideUISignal() ),cp,SLOT( deleteLater() ) ) ;
	return cp;
}

void zuluCrypt::createPartition( QString partition )
{
	setUpCreatepartition()->ShowPartition( partition ) ;
}

void zuluCrypt::FileCreated( QString file )
{
	if( utility::exists( file ) ){
		setUpCreatepartition()->ShowFile( file ) ;
	}
}

openvolume * zuluCrypt::setUpOpenpartition()
{
	openvolume * op = new openvolume( this ) ;
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) ) ;
	return op ;
}

void zuluCrypt::ShowNonSystemPartitions()
{
	openvolume * nsp = setUpOpenpartition() ;
	connect( nsp,SIGNAL( clickedPartition( QString ) ),this,SLOT( createPartition( QString ) ) ) ;
	nsp->ShowNonSystemPartitions() ;
}

void zuluCrypt::ShowOpenPartition()
{
	openvolume * ap = setUpOpenpartition() ;
	ap->showEncryptedOnly() ;
	connect( ap,SIGNAL( clickedPartition( QString ) ),this,SLOT( partitionClicked( QString ) ) ) ;
	ap->ShowAllPartitions() ;
}

passwordDialog * zuluCrypt::setUpPasswordDialog()
{
	passwordDialog * pd = new passwordDialog( m_ui->tableWidget,m_folderOpener,this ) ;
	connect( pd,SIGNAL( HideUISignal() ),pd,SLOT( deleteLater() ) ) ;
	return pd ;
}

void zuluCrypt::ShowPasswordDialog()
{
	setUpPasswordDialog()->ShowUI() ;
}

void zuluCrypt::ShowPasswordDialog( QString x,QString y )
{
	setUpPasswordDialog()->ShowUI( x,y ) ;
}

void zuluCrypt::partitionClicked( QString partition )
{
	setUpPasswordDialog()->clickedPartitionOption( partition ) ;
}

void zuluCrypt::ShowEraseDataDialog()
{
	erasedevice * ed = new erasedevice( this ) ;
	connect( ed,SIGNAL( HideUISignal() ),ed,SLOT( deleteLater() ) ) ;
	ed->ShowUI() ;
}

cryptfiles * zuluCrypt::setUpCryptFiles()
{
	cryptfiles * cf = new cryptfiles( this ) ;
	connect( cf,SIGNAL( HideUISignal() ),cf,SLOT( deleteLater() ) ) ;
	return cf ;
}

void zuluCrypt::encryptFile()
{
	setUpCryptFiles()->encrypt() ;
}

void zuluCrypt::decryptFile()
{
	setUpCryptFiles()->decrypt() ;
}

zuluCrypt::~zuluCrypt()
{
	if( m_ui ){
		delete m_ui ;
	}
	if( m_trayIcon ){
		delete m_trayIcon ;
	}
}
