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
zuluCrypt::zuluCrypt( QWidget * parent ) :QMainWindow( parent ),m_ui( new Ui::zuluCrypt ),m_trayIcon( 0 )
{
	m_folderOpener = QString( "xdg-open" ) ;
	processArgumentList();
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
	setupUIElements();
	setupConnections();
	StartUpAddOpenedVolumesToTableThread();
	initFont();
	initKeyCombo();
	initTray();
}

void zuluCrypt::processArgumentList()
{
	QStringList argv = QCoreApplication::arguments() ;

	int last_spot = argv.size() - 1 ;
	int index = argv.indexOf( "-m" ) ;

	if( index != -1 ){
		if( index < last_spot ){
			m_folderOpener = argv.at( index + 1 ) ;
		}
	}

	index = argv.indexOf( "-d" ) ;

	m_device = QString( "" ) ;

	if( index != -1 ){
		if( index < last_spot ){
			m_device = argv.at( index + 1 ) ;
		}
	}
}

void zuluCrypt::openVolumeFromArgumentList()
{

}

void zuluCrypt::initKeyCombo()
{
	QAction * rca = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Menu );
	keys.append( Qt::CTRL + Qt::Key_M );
	rca->setShortcuts( keys ) ;
	connect( rca,SIGNAL( triggered() ),this,SLOT( menuKeyPressed() ) ) ;
	this->addAction( rca );
}

void zuluCrypt::initFont()
{
	userfont f( this ) ;
	setUserFont( f.getFont() );
}

void zuluCrypt::raiseWindow()
{
	this->setVisible( true );
	this->show(); ;
	this->raise();
	this->setWindowState( Qt::WindowActive ) ;
}

void zuluCrypt::raiseWindow( QString device )
{
	this->setVisible( true );
	this->show(); ;
	this->raise();
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
		this->setUpApp();
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
		f.close();
	}

	f.open( QIODevice::ReadOnly ) ;
	QByteArray c = f.readAll() ;
	f.close() ;

	m_ui->actionTray_icon->setCheckable( true );

	if( c.at( 0 ) == '1' ){
		m_ui->actionTray_icon->setChecked( true );
		m_trayIcon->show();
	}else{
		m_ui->actionTray_icon->setChecked( false );
		m_trayIcon->hide();
	}
}

void zuluCrypt::startUpdateFinished( int st )
{
	Q_UNUSED( st ) ;
	m_ui->tableWidget->setEnabled( true );
	m_ui->tableWidget->setFocus();
	if( !m_device.isEmpty() ){
		QString y = m_device.split( "/" ).last() ;
		this->ShowPasswordDialog( m_device,y ) ;
	}
}

void zuluCrypt::StartUpAddOpenedVolumesToTableThread()
{
	m_ui->tableWidget->setEnabled( false );
	startupupdateopenedvolumes * sov = new startupupdateopenedvolumes();
	connect( sov,SIGNAL( addItemToTable( QString,QString,QString ) ),this,SLOT( addItemToTable( QString,QString,QString ) ) ) ;
	connect( sov,SIGNAL( finished( int ) ),this,SLOT( startUpdateFinished( int ) ) );
	sov->start();
}

void zuluCrypt::setupUIElements()
{
	m_trayIcon = new QSystemTrayIcon( this );
	m_trayIcon->setIcon( QIcon( QString( ":/zuluCrypt.png" ) ) );

	QMenu * trayMenu = new QMenu( this ) ;
	trayMenu->addAction( tr( "quit" ),this,SLOT( closeApplication() ) );
	m_trayIcon->setContextMenu( trayMenu );

	m_ui->setupUi( this );

	this->setFixedSize( this->size() );
	this->setWindowIcon( QIcon( QString( ":/zuluCrypt.png" ) ) );

	m_ui->tableWidget->setColumnWidth( 0,298 );
	m_ui->tableWidget->setColumnWidth( 1,336 );
	//m_ui->tableWidget->setColumnWidth( 2,70 );
}

void zuluCrypt::itemEntered( QTableWidgetItem * item )
{
	int row = item->row() ;
	QTableWidget * table = item->tableWidget() ;
	QString m_point = table->item( row,1 )->text() ;
	if( !m_point.isEmpty() ){
		item->setToolTip( utility::shareMountPointToolTip( m_point ) );
	}
}

void zuluCrypt::setupConnections()
{
	m_ui->tableWidget->setMouseTracking( true );
	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionErase_data_on_device,SIGNAL( triggered() ),this,SLOT( ShowEraseDataDialog() ) );
	connect( this,SIGNAL( favClickedVolume( QString,QString ) ),this,SLOT( ShowPasswordDialog( QString,QString ) ) ) ;
	connect( m_ui->actionPartitionOpen,SIGNAL( triggered() ),this,SLOT( ShowOpenPartition() ) );
	connect( m_ui->actionFileOpen,SIGNAL( triggered() ),this,SLOT( ShowPasswordDialog() ) ) ;
	connect( m_ui->actionFileCreate,SIGNAL( triggered() ),this,SLOT( ShowCreateFile() ) );
	connect( m_ui->actionManage_names,SIGNAL( triggered() ),this,SLOT( ShowFavoritesEntries() ) );
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->actionCreatekeyFile,SIGNAL( triggered() ),this,SLOT( ShowCreateKeyFile() ) );
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
	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( trayClicked( QSystemTrayIcon::ActivationReason ) ) );
	connect( m_ui->menuFavorites,SIGNAL( triggered( QAction * ) ),this,SLOT( favClicked( QAction * ) ) ) ;
	connect( this,SIGNAL( luksAddKey( QString ) ),this,SLOT( ShowAddKeyContextMenu( QString ) ) ) ;
	connect( this,SIGNAL( luksDeleteKey( QString ) ),this,SLOT( ShowDeleteKeyContextMenu( QString ) ) );
	connect( m_ui->action_close,SIGNAL( triggered() ),this,SLOT( closeApplication() ) ) ;
	connect( m_ui->action_minimize,SIGNAL( triggered() ),this,SLOT( minimize() ) );
	connect( m_ui->actionMinimize_to_tray,SIGNAL( triggered() ),this,SLOT( minimizeToTray() ) ) ;
	connect( m_ui->actionClose_all_opened_volumes,SIGNAL( triggered() ),this,SLOT( closeAllVolumes() ) ) ;
	connect( m_ui->actionBackup_header,SIGNAL( triggered() ),this,SLOT( luksHeaderBackUp() ) );
	connect( m_ui->actionRestore_header,SIGNAL( triggered() ),this,SLOT( luksRestoreHeader() ) );
	connect( m_ui->actionPermission_problems,SIGNAL( triggered() ),this,SLOT( permissionExplanation() ) );
	connect( m_ui->actionEncrypt_file,SIGNAL( triggered() ),this,SLOT( encryptFile() ) );
	connect( m_ui->actionDecrypt_file,SIGNAL( triggered() ),this,SLOT( decryptFile() ) );
	connect( m_ui->actionLuks_header_backup,SIGNAL( triggered() ),this,SLOT( HelpLuksHeaderBackUp() ) );
	connect( m_ui->actionManage_system_partitions,SIGNAL( triggered() ),this,SLOT( ShowManageSystemPartitions() ) ) ;
	connect( m_ui->actionManage_non_system_partitions,SIGNAL( triggered() ),this,SLOT( ShowManageNonSystemPartitions() ) ) ;
	connect( m_ui->actionManage_kwallet,SIGNAL( triggered() ),this,SLOT( manageWallet() ) ) ;
	connect( m_ui->actionUse_kde_default_wallet,SIGNAL( triggered() ),this,SLOT( setDefaultWallet() ) ) ;

	m_ui->actionUse_kde_default_wallet->setCheckable( true );

	m_ui->actionUse_kde_default_wallet->setChecked( zuluOptions::walletIsKDEWallet() );

	if( !kwalletplugin::hasFunctionality() ){
		m_ui->actionManage_kwallet->setEnabled( false ) ;
		m_ui->actionUse_kde_default_wallet->setEnabled( false );
	}

	m_ui->actionManage_system_partitions->setEnabled( utility::userIsRoot() );
	m_ui->actionManage_non_system_partitions->setEnabled( utility::userIsRoot() );

	this->setAcceptDrops( true );
}

void zuluCrypt::permissionExplanation()
{
	DialogMsg m( this ) ;
	m.ShowPermissionProblem( QString( "" ) );
}

void zuluCrypt::ShowManageSystemPartitions()
{
	manageSystemVolumes * msv = new manageSystemVolumes( this ) ;
	connect( msv,SIGNAL( HideUISignal() ),msv,SLOT(deleteLater() ) ) ;
	msv->ShowUI( QString( "/etc/zuluCrypt-system" ) ) ;
}

void zuluCrypt::ShowManageNonSystemPartitions()
{
	manageSystemVolumes * msv = new manageSystemVolumes( this ) ;
	connect( msv,SIGNAL( HideUISignal() ),msv,SLOT(deleteLater() ) ) ;
	msv->ShowUI( QString( "/etc/zuluCrypt-nonsystem" ) ) ;
}

void zuluCrypt::currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;

	int rowCount = m_ui->tableWidget->rowCount() ;

	if( rowCount > 12 ){
		m_ui->tableWidget->setColumnWidth( 2,70 );
	}else{
		m_ui->tableWidget->setColumnWidth( 2,95 );
	}
}

void zuluCrypt::closeAllVolumes()
{
	closeAllVolumesThread * cavt = new closeAllVolumesThread( m_ui->tableWidget ) ;
	connect( cavt,SIGNAL( close( QTableWidgetItem *,int ) ),this,SLOT( closeAll( QTableWidgetItem *,int ) ) ) ;
	cavt->start();
}

void zuluCrypt::closeAll( QTableWidgetItem * i,int st )
{
	st ? closeStatusErrorMessage( st ) : removeRowFromTable( i->row() );
}

void zuluCrypt::minimize()
{
	zuluCrypt::setWindowState( Qt::WindowMinimized ) ;
}

void zuluCrypt::minimizeToTray()
{
	if( m_ui->actionTray_icon->isChecked() == true ){
		this->hide();
	}else{
		m_ui->actionTray_icon->setChecked( true );
		trayProperty() ;
		this->hide();
	}
}

void zuluCrypt::closeEvent( QCloseEvent * e )
{
	if( m_trayIcon->isVisible() == true ){
		this->hide();
		e->ignore();
	}else{
		this->hide();
		e->accept();
	}
}

void zuluCrypt::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept();
}

void zuluCrypt::dropEvent( QDropEvent * e )
{
	const QMimeData * m = e->mimeData() ;
	QList<QUrl> l = m->urls() ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){
		m_device = l.at( i ).path() ;
		QString y = m_device.split( "/" ).last() ;
		this->ShowPasswordDialog( m_device,y ) ;
	}
}

void zuluCrypt::closeApplication()
{
	m_trayIcon->hide();
	this->hide() ;
	QCoreApplication::quit();
}

void zuluCrypt::trayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){
		if( this->isVisible() == true ){
			this->hide();
		}else{
			this->show();
		}
	}
}

void zuluCrypt::trayProperty()
{
	m_ui->actionTray_icon->setEnabled( false );
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/tray" ) ) ;
	f.open( QIODevice::ReadOnly ) ;
	QByteArray c = f.readAll() ;
	f.close();
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	QByteArray data ;
	if( c.at( 0 ) == '1' ){
		data.append( '0' ) ;
		f.write( data ) ;
		m_trayIcon->hide();
	}else{
		data.append( '1' ) ;
		f.write( data ) ;
		m_trayIcon->show();
	}
	f.close();
	m_ui->actionTray_icon->setEnabled( true );
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
			Font.setPointSize( k );
			UIMessage( tr( "info" ),tr( "resetting font size to %1 because larger font sizes do not fit" ).arg( QString::number( size ) ) );
		}

		setUserFont( Font );
		userfont f( this ) ;
		f.saveFont( Font );
	}
}

void zuluCrypt::setUserFont( QFont Font )
{
	this->setFont( Font );

	m_ui->tableWidget->horizontalHeaderItem( 0 )->setFont( Font );
	m_ui->tableWidget->horizontalHeaderItem( 1 )->setFont( Font );
	m_ui->tableWidget->horizontalHeaderItem( 2 )->setFont( Font );

	m_ui->actionAbout->setFont( this->font() );
	m_ui->actionAddKey->setFont( this->font() );
	m_ui->actionCreatekeyFile->setFont( this->font() );
	m_ui->actionDeleteKey->setFont( this->font() );
	m_ui->actionFavorite_volumes->setFont( this->font() );
	m_ui->actionFileCreate->setFont( this->font() );
	m_ui->actionFileOpen->setFont( this->font() );
	m_ui->actionFonts->setFont( this->font() );
	m_ui->actionInfo->setFont( this->font() );
	m_ui->actionManage_favorites->setFont( this->font() );
	m_ui->actionPartitionCreate->setFont( this->font() );
	m_ui->actionPartitionOpen->setFont( this->font() );
	m_ui->actionSelect_random_number_generator->setFont( this->font() );
	m_ui->actionTray_icon->setFont( this->font() );
	m_ui->menuFavorites->setFont( this->font() );
	m_ui->actionManage_names->setFont( this->font() );
	m_ui->actionBackup_header->setFont( this->font() );
	m_ui->actionRestore_header->setFont( this->font() );
	m_ui->actionEncrypt_file->setFont( this->font() );
	m_ui->actionDecrypt_file->setFont( this->font() );
	m_ui->menu_zc->setFont( this->font() );
	m_ui->actionPermission_problems->setFont( this->font() );
	m_ui->actionLuks_header_backup->setFont( this->font() );
	m_ui->actionManage_system_partitions->setFont( this->font() );
	m_ui->actionManage_non_system_partitions->setFont( this->font() );
	m_ui->actionManage_kwallet->setFont( this->font() );
	m_ui->actionUse_kde_default_wallet->setFont( this->font() );
}

void zuluCrypt::info()
{
	cryptoinfo * cinfo = new cryptoinfo( this );
	connect( cinfo,SIGNAL( closeUISignal() ),cinfo,SLOT( deleteLater() ) );
	cinfo->show();
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
along with this program.  If not, see <http://www.gnu.org/licenses/>." ).arg( VERSION_STRING );

	DialogMsg m( this ) ;
	m.ShowUIInfo( tr( "about zuluCrypt" ),license );
}

void zuluCrypt::HelpLuksHeaderBackUp()
{
	QString msg = QString( "\nAll luks based encrypted volumes have what is called a \"luks header\".\n\n\
A luks header is responsible for storing information necessary to open a luks based volume and any damage \
to it will makes it impossible to open the volume causing permanent loss of encrypted data.\n\n\
The damage to the header is usually caused by accidental formatting of the device or use of \
some buggy partitioning tools or wrongly reassembled logical volumes.\n\n\
Having a backup of the luks header is strongly advised because it is the only way the encrypted data will be accessible \
again after the header is restored if the header on the volume get corrupted.\n\n\
For more information, please read the FAQ at: http://code.google.com/p/cryptsetup/wiki/FrequentlyAskedQuestions" );

	DialogMsg m( this ) ;
	m.ShowUIInfo( tr( "important information on luks header" ),msg );
}

void zuluCrypt::addItemToTable( QString device,QString m_point,QString type )
{
	QStringList s ;

	QString dev = device ;
	dev.replace( "\"","\"\"\"" ) ;

	s.append( device );
	s.append( m_point );
	if( type == QString( "crypto_LUKS" ) ){
		s.append( QString( "luks" ) );
	}else if( type == QString( "crypto_PLAIN" ) ){
		s.append( QString( "plain" ) );
	}else if( type == QString( "crypto_TCRYPT" ) ){
		s.append( QString( "tcrypt" ) );
	}
	tablewidget::addRowToTable( m_ui->tableWidget,s );
}

void zuluCrypt::removeRowFromTable( int x )
{
	tablewidget::deleteRowFromTable( m_ui->tableWidget,x ) ;
}

void zuluCrypt::volume_property()
{
	m_ui->tableWidget->setEnabled( false );
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	QString x = m_ui->tableWidget->item( item->row(),0 )->text() ;
	QString y = m_ui->tableWidget->item( item->row(),1 )->text() ;
	volumePropertiesThread * vpt = new volumePropertiesThread( x,y );
	connect( vpt,SIGNAL( finished( QString ) ),this,SLOT( volumePropertyThreadFinished( QString ) ) ) ;
	vpt->start();
}

void zuluCrypt::volumePropertyThreadFinished( QString properties )
{
	DialogMsg msg( this ) ;
	if( properties.isEmpty() ){
		msg.ShowUIOK( tr( "ERROR!"),tr( "volume is not open or was opened by a different user" ) );
	}else{
		msg.ShowUIVolumeProperties( tr( "volume properties" ),properties );
	}
	m_ui->tableWidget->setEnabled(true );
}

void zuluCrypt::favAboutToHide()
{
}

void zuluCrypt::favClicked( QAction * e )
{
	QStringList l = e->text().split( "\t" ) ;
	emit favClickedVolume( l.at( 0 ),l.at( 1 ) );
}

void zuluCrypt::readFavorites()
{
	QAction * ac ;
	m_ui->menuFavorites->clear();
	QStringList l = utility::readFavorites() ;
	if( l.isEmpty() == false ){
		for( int i = 0 ; i < l.size() - 1 ; i++ ){
			ac = new QAction( l.at( i ),m_ui->menuFavorites ) ;
			m_ui->menuFavorites->addAction( ac );
		}
	}else{
		ac = new QAction( tr( "list is empty" ),m_ui->menuFavorites ) ;
		ac->setEnabled( false );
		m_ui->menuFavorites->addAction( ac );
	}
}

void zuluCrypt::addToFavorite()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	QString x = m_ui->tableWidget->item( item->row(),0 )->text() ;
	QString y = m_ui->tableWidget->item( item->row(),1 )->text() ;
	utility::addToFavorite( x,y );
}

void zuluCrypt::menuKeyPressed()
{
	QTableWidgetItem * it = m_ui->tableWidget->currentItem() ;
	itemClicked( it,false );
}

void zuluCrypt::fileManagerOpenStatus( int exitCode, int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) );
	}
}

void zuluCrypt::openFolder()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString path = m_ui->tableWidget->item( item->row(),1 )->text() ;
	openmountpointinfilemanager * omp = new openmountpointinfilemanager( m_folderOpener,path ) ;
	connect( omp,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	omp->start();
}

void zuluCrypt::itemClicked( QTableWidgetItem * it )
{
	itemClicked( it,true );
}

void zuluCrypt::itemClicked( QTableWidgetItem * item, bool clicked )
{
	QMenu m ;
	m.setFont( this->font() );
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
		m.addSeparator();
		connect( m.addAction( tr( "backup luks header" ) ),SIGNAL( triggered() ),this,SLOT( luksHeaderBackUpContextMenu() ) ) ;
	}

	m.addSeparator() ;

	QString volume_path = m_ui->tableWidget->item( item->row(),0 )->text() ;
	QString mount_point_path = m_ui->tableWidget->item( item->row(),1 )->text();

	int i = mount_point_path.lastIndexOf( "/" ) ;

	mount_point_path = mount_point_path.left( i ) ;

	QString fav = volume_path + QString( "\t" ) + mount_point_path ;

	QFile f( QDir::homePath() + QString( "/.zuluCrypt/favorites" ) ) ;

	f.open( QIODevice::ReadOnly ) ;

	QByteArray data = f.readAll() ;

	QAction a( tr( "add to favorite" ),( QObject * )&m ) ;

	m.addAction( &a );

	if( strstr( data.data() , fav.toAscii().data() ) == NULL ){
		a.setEnabled( true );
		a.connect( ( QObject * )&a,SIGNAL( triggered() ),this,SLOT( addToFavorite() ) ) ;

	}else{
		a.setEnabled( false );
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
	m_ui->actionUse_kde_default_wallet->setEnabled( false );

	if( m_ui->actionUse_kde_default_wallet->isChecked() ){
		zuluOptions::setWalletToKDEDefaultName() ;
	}else{
		zuluOptions::setWalletToDefaultName() ;
	}

	m_ui->actionUse_kde_default_wallet->setEnabled( true );
}

void zuluCrypt::luksAddKeyContextMenu( void )
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	emit luksAddKey( m_ui->tableWidget->item( item->row(),0 )->text() ) ;
}

void zuluCrypt::luksDeleteKeyContextMenu( void )
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	emit luksDeleteKey( m_ui->tableWidget->item( item->row(),0 )->text() ) ;
}

void zuluCrypt::UIMessage( QString title, QString message )
{
	DialogMsg msg( this );
	msg.ShowUIOK( title,message );
}

void zuluCrypt::closeStatus( int st )
{
	m_ui->tableWidget->setEnabled( true );
	if( st == 0 ){
		removeRowFromTable( m_ui->tableWidget->currentItem()->row() ) ;
	}else{
		closeStatusErrorMessage( st );
	}
}

void zuluCrypt::closeStatusErrorMessage( int st )
{
	switch ( st ) {
		case 1 :UIMessage( tr( "ERROR!" ),tr( "close failed, volume is not open or was opened by a different user" ) ) ;			break ;
		case 2 :UIMessage( tr( "ERROR!" ),tr( "close failed, one or more files in the volume are in use." ) );					break ;
		case 3 :UIMessage( tr( "ERROR!" ),tr( "close failed, volume does not have an entry in /etc/mtab" ) );					break ;
		case 4 :UIMessage( tr( "ERROR!" ),tr( "close failed, could not get a lock on /etc/mtab~" ) );						break ;
		case 5 :UIMessage( tr( "ERROR!" ),tr( "close failed, volume is unmounted but could not close mapper,advice to close it manually" ) );	break ;
		case 6 :UIMessage( tr( "ERROR!" ),tr( "close failed, could not resolve full path of device\n" ) );               			break ;
		case 7 :UIMessage( tr( "ERROR!" ),tr( "close failed, shared mount point appear to be busy\n" ) );					break ;
		case 8 :UIMessage( tr( "ERROR!" ),tr( "close failed, shared mount point appear to belong to a different user\n" ) );			break ;
		case 9 :UIMessage( tr( "ERROR!" ),tr( "close failed, shared mount point appear to be in an ambiguous state,advice to unmount manually" )) ;break ;
		case 110:UIMessage( tr( "ERROR!" ),tr( "close failed, could not find any partition with the presented UUID" ) );			break ;
		default:UIMessage( tr( "ERROR!" ),tr( "unrecognized error with status number %1 encountered" ).arg( st ) );
	}
}

void zuluCrypt::close()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	QString vol = m_ui->tableWidget->item( item->row(),0 )->text().replace( "\"","\"\"\"" ) ;
	QString exe = QString( ZULUCRYPTzuluCrypt ) + QString( " -q -d " ) + QString( "\"" ) + vol + QString( "\"" ) ;
	m_ui->tableWidget->setEnabled( false );
	closeVolumeThread * cvt = new closeVolumeThread( exe ) ;
	connect( cvt,SIGNAL( finished( int ) ),this,SLOT( closeStatus( int ) ) ) ;
	cvt->start();
}

manageluksheader * zuluCrypt::setUpManageLuksHeader()
{
	manageluksheader * bkh = new manageluksheader( this );
	connect( bkh,SIGNAL( HideUISignal() ),bkh,SLOT( deleteLater() ) );
	return bkh ;
}

void zuluCrypt::luksRestoreHeader()
{
	this->setUpManageLuksHeader()->restoreHeader();
}

void zuluCrypt::luksHeaderBackUp()
{
	this->setUpManageLuksHeader()->backUpHeader();
}

void zuluCrypt::luksHeaderBackUpContextMenu()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem();
	QString device = m_ui->tableWidget->item( item->row(),0 )->text() ;
	this->setUpManageLuksHeader()->backUpHeader( device );
}

luksaddkey * zuluCrypt::setUpluksaddkey()
{
	luksaddkey * addKey = new luksaddkey( this );
	connect( addKey,SIGNAL( HideUISignal() ),addKey,SLOT( deleteLater() ) );
	return addKey ;
}

void zuluCrypt::ShowAddKeyContextMenu( QString key )
{
	setUpluksaddkey()->ShowUI( key );
}

void zuluCrypt::ShowAddKey()
{
	setUpluksaddkey()->ShowUI();
}

luksdeletekey * zuluCrypt::setUpluksdeletekey()
{
	luksdeletekey * deleteKey = new luksdeletekey( this );
	connect( deleteKey,SIGNAL( HideUISignal() ),deleteKey,SLOT( deleteLater() ) );
	return deleteKey ;
}

void zuluCrypt::ShowDeleteKeyContextMenu( QString key )
{
	setUpluksdeletekey()->ShowUI( key );
}

void zuluCrypt::ShowDeleteKey()
{
	setUpluksdeletekey()->ShowUI();
}

void zuluCrypt::ShowCreateKeyFile()
{
	createkeyfile * ckf = new createkeyfile( this );
	connect( ckf,SIGNAL( HideUISignal() ),ckf,SLOT( deleteLater() ) );
	ckf->ShowUI();
}

void zuluCrypt::ShowFavoritesEntries()
{
	managedevicenames * mdn = new managedevicenames( this );
	connect( mdn,SIGNAL( HideUISignal() ),mdn,SLOT( deleteLater() ) );
	mdn->ShowUI();
}

void zuluCrypt::ShowCreateFile()
{
	createfile * cf = new createfile( this );
	connect( cf,SIGNAL( HideUISignal() ),cf,SLOT( deleteLater() ) );
	connect( cf,SIGNAL( fileCreated( QString ) ),this,SLOT( FileCreated( QString ) ) );
	cf->showUI();
}

createvolume * zuluCrypt::setUpCreatepartition()
{
	createvolume * cp = new createvolume( this );
	connect( cp,SIGNAL( HideUISignal() ),cp,SLOT( deleteLater() ) );
	return cp;
}

void zuluCrypt::createPartition( QString partition )
{
	setUpCreatepartition()->ShowPartition( partition );
}

void zuluCrypt::FileCreated( QString file )
{
	setUpCreatepartition()->ShowFile( file );
}

openvolume * zuluCrypt::setUpOpenpartition()
{
	openvolume * op = new openvolume( this );
	connect( op,SIGNAL( HideUISignal() ),op,SLOT( deleteLater() ) );
	return op ;
}

void zuluCrypt::ShowNonSystemPartitions()
{
	openvolume * nsp = setUpOpenpartition() ;
	connect( nsp,SIGNAL( clickedPartition( QString ) ),this,SLOT( createPartition( QString ) ) );
	nsp->ShowNonSystemPartitions();
}

void zuluCrypt::ShowOpenPartition()
{
	openvolume * ap = setUpOpenpartition() ;
	ap->showEncryptedOnly();
	connect( ap,SIGNAL( clickedPartition( QString ) ),this,SLOT( partitionClicked( QString ) ) );
	ap->ShowAllPartitions();
}

passwordDialog * zuluCrypt::setUpPasswordDialog()
{
	passwordDialog * pd = new passwordDialog( m_ui->tableWidget,m_folderOpener,this ) ;
	connect( pd,SIGNAL( HideUISignal() ),pd,SLOT( deleteLater() ) );
	return pd ;
}

void zuluCrypt::ShowPasswordDialog()
{
	setUpPasswordDialog()->ShowUI();
}

void zuluCrypt::ShowPasswordDialog( QString x, QString y )
{
	setUpPasswordDialog()->ShowUI( x,y );
}

void zuluCrypt::partitionClicked( QString partition )
{
	setUpPasswordDialog()->clickedPartitionOption( partition );
}

void zuluCrypt::ShowEraseDataDialog()
{
	erasedevice * ed = new erasedevice( this ) ;
	connect( ed,SIGNAL( HideUISignal() ),ed,SLOT( deleteLater() ) );
	ed->ShowUI();
}

cryptfiles * zuluCrypt::setUpCryptFiles()
{
	cryptfiles * cf = new cryptfiles( this );
	connect( cf,SIGNAL( HideUISignal() ),cf,SLOT( deleteLater() ) ) ;
	return cf ;
}

void zuluCrypt::encryptFile()
{
	setUpCryptFiles()->encrypt();
}

void zuluCrypt::decryptFile()
{
	setUpCryptFiles()->decrypt();
}

void zuluCrypt::failedToOpenWallet()
{
	//DialogMsg msg( this ) ;
	//msg.ShowUIOK( QString( "ERROR" ),QString( "could not open selected wallet" ) ) ;
}

void zuluCrypt::manageWallet()
{
	if( kwalletplugin::KwalletIsEnabled() ){
		kwalletconfig * cfg = new kwalletconfig( this ) ;
		connect( cfg,SIGNAL( HideUISignal() ),cfg,SLOT( deleteLater() ) ) ;
		connect( cfg,SIGNAL( couldNotOpenWallet() ),this,SLOT( failedToOpenWallet() ) ) ;
		cfg->ShowUI();
	}else{
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "kwallet functionality doesnt seem to be enabled" ) ) ;
	}
}

zuluCrypt::~zuluCrypt()
{
	if( m_ui ){
		delete m_ui;
	}
	if( m_trayIcon ){
		delete m_trayIcon ;
	}
}
