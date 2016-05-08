/*
 *
 *  Copyright (c) 2012-2015
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

#include "zulumount.h"
#include "ui_zulumount.h"
#include <QDebug>

#include <QMainWindow>

#include <QTableWidgetItem>
#include <QDir>
#include <QIcon>
#include <QAction>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QFont>
#include <QCursor>
#include <QAction>
#include <QFileDialog>
#include <QUrl>
#include <QTranslator>
#include <QMimeData>
#include <QFile>

#include <utility>
#include <initializer_list>

#include <unistd.h>
#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"
#include "oneinstance.h"
#include "events.h"
#include "monitor_mountinfo.h"
#include "../zuluCrypt-gui/utility.h"
#include "zulumounttask.h"
#include "../zuluCrypt-gui/task.h"
#include "../zuluCrypt-gui/checkforupdates.h"
#include "../zuluCrypt-gui/favorites.h"

#include <memory>

zuluMount::zuluMount( QWidget * parent ) :
	QWidget( parent ),
	m_mountInfo( monitor_mountinfo::instance( this,true,[ this ](){ this->quitApplication() ; } ) ),
	m_events( events::instance( this,m_mountInfo.stop() ) )
{
}

void zuluMount::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage( true ) ;

	m_ui = new Ui::zuluMount ;
	m_ui->setupUi( this ) ;

	m_ui->pbunlockcryptfs->setMinimumHeight( 31 ) ;
	m_ui->pbmenu->setMinimumHeight( 31 ) ;
	m_ui->pbmount->setMinimumHeight( 31 ) ;
	m_ui->pbupdate->setMinimumHeight( 31 ) ;

	auto f = utility::getWindowDimensions( "zuluMount" ) ;

	auto e = f.data() ;

	this->window()->setGeometry( *( e + 0 ),*( e + 1 ),*( e + 2 ),*( e + 3 ) ) ;

	auto table = m_ui->tableWidget ;

	table->setColumnWidth( 0,*( e + 4 ) ) ;
	table->setColumnWidth( 1,*( e + 5 ) ) ;
	table->setColumnWidth( 2,*( e + 6 ) ) ;
	table->setColumnWidth( 4,*( e + 7 ) ) ;
	table->setColumnWidth( 5,*( e + 8 ) ) ;

	m_ui->tableWidget->hideColumn( 3 ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	m_ui->tableWidget->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents ) ;
#else
	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
#endif
	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),
		 this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbmount,SIGNAL( clicked() ),
		 this,SLOT( pbMount() ) ) ;

	connect( m_ui->pbupdate,SIGNAL( clicked()),
		 this,SLOT( pbUpdate() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbunlockcryptfs,SIGNAL( clicked() ),
		 this,SLOT( unlockCryptFs() ) ) ;

	connect( this,SIGNAL( unlistVolume( QString ) ),
		 this,SLOT( removeVolume( QString ) ) ) ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	const auto& icon = utility::getIcon( "zuluMount" ) ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( icon ) ;

	m_trayIcon.setParent( this ) ;
	m_trayIcon.setIcon( icon ) ;

	auto trayMenu = new QMenu( this ) ;

	trayMenu->setFont( this->font() ) ;

	m_autoMount = this->autoMount() ;
	m_autoOpenFolderOnMount = this->autoOpenFolderOnMount() ;

	trayMenu->addAction( [ this ](){

		m_autoMountAction = new QAction( tr( "Automount Volumes" ),this ) ;

		m_autoMountAction->setCheckable( true ) ;
		m_autoMountAction->setChecked( m_autoMount ) ;

		connect( m_autoMountAction,SIGNAL( toggled( bool ) ),this,SLOT( autoMountToggled( bool ) ) ) ;

		return m_autoMountAction ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto e = new QAction( tr( "Auto Open Mount Point" ),this ) ;

		e->setCheckable( true ) ;
		e->setChecked( m_autoOpenFolderOnMount ) ;

		connect( e,SIGNAL( toggled( bool ) ),this,SLOT( autoOpenFolderOnMount( bool ) ) ) ;

		return e ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Unmount All" ),this ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( unMountAll() ) ) ;

		return ac ;
	}() ) ;

	m_favorite_menu = trayMenu->addMenu( tr( "Favorites" ) ) ;

	m_favorite_menu->setFont( this->font() ) ;

	connect( m_favorite_menu,SIGNAL( triggered( QAction * ) ),
		 this,SLOT( favoriteClicked( QAction * ) ) ) ;

	connect( m_favorite_menu,SIGNAL( aboutToShow() ),
		 this,SLOT( showFavorites() ) ) ;

	m_not_hidden_volume_menu = trayMenu->addMenu( tr( "Hide Volume From View" ) ) ;

	m_not_hidden_volume_menu->setFont( this->font() ) ;

	connect( m_not_hidden_volume_menu,SIGNAL( triggered( QAction * ) ),
		 this,SLOT( removeVolumeFromVisibleVolumeList( QAction * ) ) ) ;

	connect( m_not_hidden_volume_menu,SIGNAL( aboutToShow() ),
		 this,SLOT( showVisibleVolumeList() ) ) ;

	m_hidden_volume_menu = trayMenu->addMenu( tr( "Unhide Volume From View" ) ) ;

	m_hidden_volume_menu->setFont( this->font() ) ;

	connect( m_hidden_volume_menu,SIGNAL( triggered( QAction * ) ),
		 this,SLOT( removeVolumeFromHiddenVolumeList( QAction * ) ) ) ;

	connect( m_hidden_volume_menu,SIGNAL( aboutToShow() ),
		 this,SLOT( showHiddenVolumeList() ) ) ;

	m_languageAction = new QAction( this ) ;
	m_languageAction->setText( tr( "Select Language" ) ) ;

	trayMenu->addAction( m_languageAction ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "Check For Update" ),this ) ;
		
		connect( ac,SIGNAL( triggered() ),this,SLOT( updateCheck() ) ) ;

		return ac ;
	}() ) ;

	trayMenu->addAction( [ this ](){

		auto ac = new QAction( tr( "About" ),this ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( licenseInfo() ) ) ;

		return ac ;
	}() ) ;

	trayMenu->addAction( tr( "Quit" ),this,SLOT( closeApplication() ) ) ;
	m_trayIcon.setContextMenu( trayMenu ) ;

	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;

	m_ui->pbmenu->setMenu( m_trayIcon.contextMenu() ) ;

	this->setLocalizationLanguage( false ) ;

	m_trayIcon.show() ;

	auto dirPath = utility::homePath() + "/.zuluCrypt/" ;
	QDir dir( dirPath ) ;

	if( !dir.exists() ){

		dir.mkdir( dirPath ) ;
	}

	m_powerOff = utility::powerOffCommand() ;

	this->disableAll() ;

	this->startAutoMonitor() ;

	this->updateVolumeList( zuluMountTask::updateVolumeList().await() ) ;

	if( volume.isEmpty() ) {

		this->enableAll() ;
	}else{
		this->showMoungDialog( volume ) ;
	}

	this->autoUpdateCheck() ;
}

void zuluMount::licenseInfo()
{
	utility::licenseInfo( this ) ;
}

void zuluMount::updateCheck()
{
	checkForUpdates::instance( this ) ;
}

void zuluMount::autoUpdateCheck()
{
	checkForUpdates::instance( this,"zuluMount" ) ;
}

void zuluMount::removeVolumeFromHiddenVolumeList( QAction * ac )
{
	this->disableAll() ;

	auto e = ac->text() ;
	e.remove( "&" ) ;

	zuluMountTask::removeVolumeFromHiddenVolumeList( e ) ;

	this->updateList( zuluMountTask::getVolumeProperties( e ).await() ) ;

	this->enableAll() ;
}

static void _manage_volume_list( QMenu * menu,const QStringList& l )
{
	menu->clear() ;

	if( l.isEmpty() ){

		menu->addAction( [ &menu ](){

			auto ac = new QAction( QObject::tr( "List Is Empty" ),menu ) ;

			ac->setEnabled( false ) ;

			return ac ;
		}() ) ;
	}else{
		for( const auto& it : l ){

			menu->addAction( new QAction( it,menu ) ) ;
		}
	}
}

void zuluMount::showHiddenVolumeList()
{
	_manage_volume_list( m_hidden_volume_menu,zuluMountTask::hiddenVolumeList() ) ;
}

void zuluMount::showVisibleVolumeList()
{
	_manage_volume_list( m_not_hidden_volume_menu,
			     tablewidget::tableColumnEntries( m_ui->tableWidget ) ) ;
}

void zuluMount::removeVolumeFromVisibleVolumeList( QAction * ac )
{
	auto e = ac->text() ;
	e.remove( "&" ) ;

	auto table = m_ui->tableWidget ;

	zuluMountTask::addVolumeToHiddenVolumeList( e ) ;

	this->disableAll() ;

	tablewidget::selectRow( table,e ) ;
	utility::Task::suspendForOneSecond() ;
	tablewidget::deleteTableRow( table,e ) ;

	tablewidget::selectLastRow( table ) ;

	this->enableAll() ;
}

QString zuluMount::resolveFavoriteMountPoint( const QString& e )
{
	for( const auto& it : utility::readFavorites() ){

		if( it.startsWith( e + '\t' ) ){

			auto l = it.split( '\t' ) ;

			if( l.size() > 1 ){

				return l.at( 1 ) ;
			}else{
				return QString() ;
			}
		}
	}

	return QString() ;
}

void zuluMount::favoriteClicked( QAction * ac )
{
	auto e = ac->text() ;
	e.remove( "&" ) ;

	auto r = utility::favoriteClickedOption( e ) ;

	if( r == 1 ){

		favorites::instance( this,true ) ;

	}else if( r == 2 ){

		for( const auto& it : utility::readFavorites() ){

			auto e = utility::split( it,'\t' ) ;

			if( e.size() > 1 ){

				this->showMoungDialog( e.at( 0 ),e.at( 1 ) ) ;
			}
		}
	}else{
		this->showMoungDialog( e,this->resolveFavoriteMountPoint( e ) ) ;
	}
}

void zuluMount::showFavorites()
{
	utility::readFavorites( m_favorite_menu,true ) ;
}

void zuluMount::setLocalizationLanguage( bool translate )
{
	utility::setLocalizationLanguage( translate,this,m_languageAction,"zuluMount-gui" ) ;
}

void zuluMount::languageMenu( QAction * ac )
{
	utility::languageMenu( this,m_languageAction->menu(),ac,"zuluMount-gui" ) ;
}

void zuluMount::autoOpenFolderOnMount( bool e )
{
	m_autoOpenFolderOnMount = e ;

	utility::autoOpenFolderOnMount( "zuluMount-gui",e ) ;
}

bool zuluMount::autoOpenFolderOnMount( void )
{
	return utility::autoOpenFolderOnMount( "zuluMount-gui" ) ;
}

void zuluMount::startAutoMonitor()
{
	m_mountInfo.start() ;
	m_events.start() ;
}

/*
 * This should be the only function that closes the application
 */
void zuluMount::closeApplication()
{
	m_events.stop() ;
}

void zuluMount::quitApplication()
{
	QCoreApplication::quit() ;
}

void zuluMount::autoMountVolume( volumeEntryProperties * q )
{
	std::unique_ptr< volumeEntryProperties > r( q ) ;

	if( r && r->isValid() ){

		auto& p = *r ;

		if( p.encryptedVolume() ){

			this->addEntryToTable( true,p ) ;
		}else{
			if( m_autoMount ){

				this->disableAll() ;

				mountPartition::instance( this,m_ui->tableWidget,[](){},[ this ]( const QString& e ){

					this->openMountPointPath( e ) ;

				} ).AutoMount( p.setMountPoint( this->resolveFavoriteMountPoint( p.volumeName() ) ) ) ;

			}else{
				this->addEntryToTable( false,p ) ;
			}
		}
	}
}

void zuluMount::volumeRemoved( QString volume )
{
	if( !volume.isEmpty() ){

		auto table = m_ui->tableWidget ;

		auto row = tablewidget::columnHasEntry( table,volume ) ;

		if( row != -1 ){

			tablewidget::deleteRowFromTable( table,row ) ;
			/*
			* see if a user just removed the device without properly closing it/unmounting it
			* and try to do so for them
			*/

			Task::exec( [ volume ](){ zuluMountTask::checkUnMount( volume ) ; } ) ;

			this->enableAll() ;
		}
	}
}

void zuluMount::removeVolume( QString volume )
{
	if( volume.isEmpty() ){

		tablewidget::selectLastRow( m_ui->tableWidget ) ;
		this->enableAll() ;
	}else{
		tablewidget::deleteTableRow( m_ui->tableWidget,volume ) ;
	}
}

void zuluMount::itemEntered( QTableWidgetItem * item )
{
	auto row = item->row() ;
	auto table = item->tableWidget() ;
	auto m_point = table->item( row,1 )->text() ;

	using string_t = decltype( table->item( row,3 )->text() ) ;

	string_t x = table->item( row,3 )->text() ;
	string_t z ;
	string_t y ;

	if( m_point == "/" ){
		/*
		 * we dont check if root path is publicly shared because the path it will produce (/run/media/public/)
		 * will always return true,a solution is to examine /proc/self/mountinfo and thats work for another day
		 */
		if( x == "Nil" ){
			x.clear() ;
		}
		z += tr( "LABEL=\"%1\"" ).arg( x ) ;

	}else if( m_point == "Nil" ){

		/*
		 * volume is not mounted,cant know its LABEL value
		 */

		x.clear() ;

		z += tr( "LABEL=\"%1\"" ).arg( x ) ;
	}else{
		if( x == "Nil" ){
			x.clear() ;
		}

		y = utility::shareMountPointToolTip( m_point ) ;

		if( y.isEmpty() ){

			z += tr( "LABEL=\"%1\"" ).arg( x ) ;
		}else{
			z += tr( "LABEL=\"%1\"\n%2" ).arg( x,y ) ;
		}
	}
	item->setToolTip( z ) ;
}

void zuluMount::startGUI()
{
	if( !m_startHidden ){
		this->raiseWindow() ;
	}
}

void zuluMount::raiseWindow()
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
}

void zuluMount::raiseWindow( QString volume )
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
	this->showMoungDialog( volume ) ;
}

void zuluMount::Show()
{
	auto l = QCoreApplication::arguments() ;

	m_startHidden  = l.contains( "-e" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;
	m_env          = utility::cmdArgumentValue( l,"-z","" ) ;

	utility::setUID( utility::cmdArgumentValue( l,"-K","-1" ).toInt() ) ;

	auto volume = utility::cmdArgumentValue( l,"-d" ) ;

	oneinstance::instance( this,"zuluMount-gui.socket","startGUI",volume,[ this,volume ]( QObject * instance ){

		connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
		connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;

		this->setUpApp( volume ) ;
	} ) ;
}

void zuluMount::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() ) ;

	auto _text = [ this ]( int row,int column ){

		return m_ui->tableWidget->item( row,column )->text() ;
	} ;

	auto row = item->row() ;

	auto device = _text( row,0 ) ;

	auto mt = _text( row,1 ) ;

	auto fs = _text( row,2 ) ;

	auto _properties_menu = [ & ]( const QString& fs,bool addSeparator ){

		if( fs != "encfs" && fs != "cryfs" ){

			connect( m.addAction( tr( "Properties" ) ),SIGNAL( triggered() ),
				 this,SLOT( volumeProperties() ) ) ;

			if( addSeparator ){

				m.addSeparator() ;
			}
		}
	} ;

	if( mt == "Nil" ){

		connect( m.addAction( tr( "Mount" ) ),SIGNAL( triggered() ),this,SLOT( slotMount() ) ) ;
	}else{
		auto mp   = QString( "/run/media/private/%1/" ).arg( utility::userName() ) ;
		auto mp_1 = QString( "/home/%1/" ).arg( utility::userName() ) ;

		if( mt.startsWith( mp ) || mt.startsWith( mp_1 ) ){

			connect( m.addAction( tr( "Unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;

			if( !m_powerOff.isEmpty() && fs != "encfs" && fs != "cryfs" && ( device.startsWith( "/dev/sd" ) || device.startsWith( "/dev/hd" ) ) ){

				connect( m.addAction( tr( "Unmount + Power Down" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount_powerDown() ) ) ;
			}

			m.addSeparator() ;

			_properties_menu( fs,true ) ;

			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;

			if( m_sharedFolderPath.isEmpty() ){

				connect( m.addAction( tr( "Open Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenFolder() ) ) ;
			}else{
				connect( m.addAction( tr( "Open Private Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenFolder() ) ) ;
				connect( m.addAction( tr( "Open Shared Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}else{
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;

			if( m_sharedFolderPath.isEmpty() ){

				if( utility::pathIsReadable( mt ) ){

					_properties_menu( fs,true ) ;

					connect( m.addAction( tr( "Open Folder" ) ),SIGNAL( triggered() ),
						 this,SLOT( slotOpenFolder() ) ) ;
				}else{
					_properties_menu( fs,false ) ;
				}
			}else{
				_properties_menu( fs,true ) ;

				connect( m.addAction( tr( "Open Shared Folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}
	}

	m.addSeparator() ;
	m.addAction( tr( "Close Menu" ) ) ;

	if( itemClicked ){
		m.exec( QCursor::pos() ) ;
	}else{
		auto p = this->pos() ;
		auto x = p.x() + 100 + m_ui->tableWidget->columnWidth( 0 ) ;
		auto y = p.y() + 50 + m_ui->tableWidget->rowHeight( 0 ) * item->row() ;
		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

void zuluMount::itemClicked( QTableWidgetItem * item )
{
	this->showContextMenu( item,true ) ;
}

void zuluMount::defaultButton()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = m_ui->tableWidget->currentRow() ;
		auto mt = m_ui->tableWidget->item( row,1 )->text() ;

		if( mt == "Nil" ){

			this->slotMount() ;
		}else{
			this->showContextMenu( m_ui->tableWidget->currentItem(),false ) ;
		}
	}
}

void zuluMount::slotOpenSharedFolder()
{
	this->openMountPoint( m_sharedFolderPath ) ;
}

void zuluMount::slotOpenFolder()
{
	auto item = m_ui->tableWidget->currentItem() ;
	auto path = m_ui->tableWidget->item( item->row(),1 )->text() ;

	this->openMountPoint( path ) ;
}

void zuluMount::openMountPoint( const QString& m_point )
{
	auto x = tr( "Warning" ) ;
	auto y = tr( "Could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ;

	utility::openPath( m_point,m_folderOpener,m_env,this,x,y ) ;
}

void zuluMount::openMountPointPath( QString m )
{
	if( m_autoOpenFolderOnMount ){

		this->openMountPoint( m ) ;
	}
}

void zuluMount::volumeProperties()
{
	this->disableAll() ;

	auto volume     = m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),0 )->text() ;
	auto volumeType = m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),2 )->text() ;

	auto r = zuluMountTask::volumeProperties( volume,volumeType ).await() ;

	DialogMsg msg( this ) ;

	if( r.isEmpty() ){

		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
	}else{
		auto i = r.indexOf( "\n" ) ;

		if( i != -1 ){
			msg.ShowUIVolumeProperties( tr( "Volume Properties" ),r.mid( i + 1 ) ) ;
		}else{
			msg.ShowUIOK( tr( "ERROR" ),
				      tr( "Could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
		}
	}

	this->enableAll() ;
}

void zuluMount::setUpShortCuts()
{
	auto ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	auto qa = new QAction( this ) ;
	QList<QKeySequence> z ;
	z.append( Qt::Key_M ) ;
	qa->setShortcuts( z ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbMount() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> p ;
	p.append( Qt::Key_U ) ;
	qa->setShortcuts( p ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> q ;
	q.append( Qt::Key_R ) ;
	qa->setShortcuts( q ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUpdate() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> e ;
	e.append( Qt::Key_C ) ;
	qa->setShortcuts( e ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( closeApplication() ) ) ;
	this->addAction( qa ) ;
}

void zuluMount::setUpFont()
{
	this->setFont( utility::getFont( this ) ) ;
}

void zuluMount::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}

void zuluMount::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){

		if( this->isVisible() ){

			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void zuluMount::autoMountToggled( bool opt )
{
	m_autoMount = opt ;
}

void zuluMount::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void zuluMount::dropEvent( QDropEvent * e )
{
	for( const auto& it : e->mimeData()->urls() ){

		this->showMoungDialog( it.path() ) ;
	}
}

void zuluMount::mount( const volumeEntryProperties& entry )
{
	this->disableAll() ;

	if( entry.encryptedVolume() ){

		keyDialog::instance( this,m_ui->tableWidget,entry,[ this ](){

			this->enableAll() ;

		},[ this ]( const QString& e ){

			this->openMountPointPath( e ) ;

		} ).ShowUI() ;
	}else{
		mountPartition::instance( this,m_ui->tableWidget,[ this ](){

			this->enableAll() ;

		},[ this ]( const QString& e ){

			this->openMountPointPath( e ) ;

		} ).ShowUI( entry ) ;
	}
}

void zuluMount::slotMount()
{
	auto table = m_ui->tableWidget ;
	auto row = table->currentRow() ;

	this->mount( tablewidget::tableRowEntries( table,row ) ) ;
}

void zuluMount::showMoungDialog( const volumeEntryProperties& v )
{
	if( v.isEmpty() ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Permission to access the volume was denied\nor\nthe volume is not supported\n(LVM/MDRAID signatures found)" ) ) ;
		this->enableAll() ;
	}else{
		this->mount( v ) ;
	}
}

void zuluMount::showMoungDialog( const QString& volume,const QString& m_point )
{
	if( !volume.isEmpty() ){

		if( utility::pathPointsToAFolder( volume ) ){

			this->mount( { volume,m_point,"cryptfs","Nil","Nil","Nil" } ) ;
		}else{
			this->disableAll() ;

			this->showMoungDialog( zuluMountTask::getVolumeProperties( volume ).await().setMountPoint( m_point ) ) ;
		}
	}
}

void zuluMount::pbMount()
{
	this->disableAll() ;

	auto path = QFileDialog::getOpenFileName( this,tr( "Select An Image File To Mount" ),utility::homePath() ) ;

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->showMoungDialog( path ) ;
	}
}

void zuluMount::unlockCryptFs()
{
	this->disableAll() ;

	auto path = QFileDialog::getExistingDirectory( this,tr( "Select An Encfs Volume Directory" ),utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->showMoungDialog( path ) ;
	}
}

QFont zuluMount::getSystemVolumeFont()
{
	auto f = this->font() ;
	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;
	return f ;
}

void zuluMount::addEntryToTable( bool systemVolume,const QStringList& l )
{
	if( systemVolume ){

		tablewidget::addRowToTable( m_ui->tableWidget,l,this->getSystemVolumeFont() ) ;
	}else{
		tablewidget::addRowToTable( m_ui->tableWidget,l ) ;
	}
}

void zuluMount::addEntryToTable( bool systemVolume,const volumeEntryProperties& e )
{
	this->addEntryToTable( systemVolume,e.entryList() ) ;
}

void zuluMount::removeEntryFromTable( QString volume )
{
	auto table = m_ui->tableWidget ;

	auto r = tablewidget::columnHasEntry( table,volume ) ;

	if( r != -1 ){

		tablewidget::deleteRowFromTable( table,r ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void zuluMount::volumeMiniProperties( bool valid,volumeEntryProperties * e )
{
	std::unique_ptr< volumeEntryProperties > volumeInfo( e ) ;

	this->disableAll() ;

	if( valid ){

		this->updateList( *volumeInfo ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void zuluMount::volumeMiniProperties( volumeEntryProperties * volumeInfo )
{
	this->volumeMiniProperties( volumeInfo,volumeInfo ) ;
}

void zuluMount::volumeMiniProperties_0( volumeEntryProperties * volumeInfo )
{
	this->volumeMiniProperties( volumeInfo && volumeInfo->mounted(),volumeInfo ) ;
}

void zuluMount::updateList( const volumeEntryProperties& entry )
{
	if( entry.isValid() ){

		auto table = m_ui->tableWidget ;

		auto row = tablewidget::columnHasEntry( table,entry.volumeName() ) ;

		if( row == -1 ){

			row = tablewidget::addEmptyRow( table ) ;
		}
		if( entry.isSystem() ){

			tablewidget::updateRowInTable( table,entry.entryList(),row,this->getSystemVolumeFont() ) ;
		}else{
			tablewidget::updateRowInTable( table,entry.entryList(),row,this->font() ) ;
		}

		tablewidget::selectRow( table,row ) ;
	}
}

bool zuluMount::errorNotFound( int r )
{
	DialogMsg m( this ) ;

	switch ( r ) {

		case 0 :break ;
		case 1 :m.ShowUIOK( tr( "ERROR!" ),tr( "Volume is not open or was opened by a different user" ) ) ;					break ;
		case 2 :m.ShowUIOK( tr( "ERROR!" ),tr( "One or more files in the volume are in use." ) ) ;						break ;
		case 3 :m.ShowUIOK( tr( "ERROR!" ),tr( "Volume does not have an entry in /etc/mtab" ) ) ;						break ;
		case 4 :m.ShowUIOK( tr( "ERROR!" ),tr( "Could not get a lock on /etc/mtab~" ) ) ;							break ;
		case 5 :m.ShowUIOK( tr( "ERROR!" ),tr( "Volume is unmounted but could not close mapper,advice to close it manually" ) ) ;		break ;
		case 6 :m.ShowUIOK( tr( "ERROR!" ),tr( "Could not resolve full path of device\n" ) ) ;							break ;
		case 7 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be busy\n" ) ) ;							break ;
		case 8 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to belong to a different user or multiple mount points detected\n" ) ) ; break ;
		case 9 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be in an ambiguous state,advice to unmount manually" ) ) ;		break ;
		case 10:m.ShowUIOK( tr( "ERROR!" ),tr( "Multiple mount points for the volume detected" ) ) ;						break ;
		case 100 :m.ShowUIOK( tr( "ERROR!" ),tr( "Device does not appear to be mounted" ) ) ;							break ;
		case 101 :m.ShowUIOK( tr( "ERROR!" ),tr( "Only root user of members of group \"zulumount\" can unmount this volume" ) ) ;               break ;
		case 107 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be busy" ) ) ;							break ;
		case 108 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to belong to a different user" ) ) ;				break ;
		case 109 :m.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point appear to be in an ambiguous state,advice to unmount manually" ) ) ;       break ;
		case 103 :m.ShowUIOK( tr( "ERROR!" ),tr( "Device does not exist" ) ) ;									break ;
		case 104 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount,the mount point and/or one or more files are in use" ) ) ;                  break ;
		case 105 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount,could not get a lock on /etc/mtab~" ) ) ;					break ;
		case 106 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount the partition" ) ) ;							break ;
		case 111 :m.ShowUIOK( tr( "ERROR!" ),tr( "Failed to unmount,multiple mount points for the volume detected" ) ) ;			break ;
		case 110 :m.ShowUIOK( tr( "ERROR!" ),tr( "Close failed, could not find any partition with the presented UUID" ) ) ;			break ;
		default: return true ;
	}

	return false ;
}

void zuluMount::unmount( const QString& e )
{
	this->disableAll() ;

	auto row = m_ui->tableWidget->currentRow() ;

	auto path = m_ui->tableWidget->item( row,0 )->text() ;
	auto type = m_ui->tableWidget->item( row,2 )->text() ;

	if( type == "encfs" || type == "cryfs" ){

		auto m = m_ui->tableWidget->item( row,1 )->text() ;

		if( !zuluMountTask::encryptedFolderUnMount( m ).await() ){

			DialogMsg m( this ) ;
			m.ShowUIOK( tr( "ERROR" ),tr( "Failed to unmount encfs/cryfs volume" ) ) ;
			this->enableAll() ;
		}
	}else{
		auto r = zuluMountTask::unmountVolume( path,type,e ).await() ;

		if( r.failed() ){

			if( this->errorNotFound( r.exitCode() ) ){

				DialogMsg m( this ) ;

				QString z = r.output() ;

				z.replace( tr( "ERROR: " ),"" ) ;
				z.replace( "ERROR: ","" ) ;

				m.ShowUIOK( tr( "ERROR" ),z ) ;
			}

			this->enableAll() ;
		}
	}
}

void zuluMount::pbUmount()
{
	this->unmount() ;
}

void zuluMount::pbUmount_powerDown()
{
	this->unmount( m_powerOff ) ;
}

void zuluMount::unMountAll()
{
	this->disableAll() ;

	auto table = m_ui->tableWidget ;

	using list_t = decltype( tablewidget::tableColumnEntries( table,1 ) ) ;

	list_t x = tablewidget::tableColumnEntries( table,1 ) ;
	list_t y = tablewidget::tableColumnEntries( table,0 ) ;
	list_t z = tablewidget::tableColumnEntries( table,2 ) ;

	list_t p ;
	list_t q ;
	list_t n ;

	auto a = utility::userName() ;
	auto b = utility::mountPath( QString() ) ;
	auto c = utility::homeMountPath( QString() ) ;

	auto k = x.size() ;

	for( decltype( k ) i = 0 ; i < k ; i++ ){

		const auto& e = x.at( i ) ;

		if( e.startsWith( b ) || e.startsWith( c ) ){

			p.append( y.at( i ) ) ;
			q.append( z.at( i ) ) ;
			n.append( e ) ;
		}
	}

	m_removeAllVolumes = true ;

	Task::await( [ & ](){

		if( p.isEmpty() ){

			utility::Task::waitForOneSecond() ;
		}else{
			for( decltype( p.size() ) i = p.size() - 1 ; i >= 0 ; i-- ){

				const auto& e = q.at( i ) ;

				if( e == "encfs" || e == "cryfs" ){

					zuluMountTask::encryptedFolderUnMount( n.at( i ) ).get() ;
				}else{
					zuluMountTask::volumeUnmount( p.at( i ),e ) ;
				}

				utility::Task::waitForOneSecond() ;
			}

			utility::Task::waitForTwoSeconds() ;
		}
	} ) ;

	this->enableAll_1() ;
}

void zuluMount::pbUpdate()
{
	this->disableAll() ;

	auto r = zuluMountTask::updateVolumeList().await() ;

	this->updateVolumeList( r ) ;

	this->removeDisappearedEntries( r ) ;
}

void zuluMount::updateVolumeList( const QVector< volumeEntryProperties >& r )
{
	if( r.isEmpty() ){

		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "Reading partition properties took longer than expected and operation was terminated,click refresh to try again" ) ) ;

		this->enableAll() ;
	}else{
		for( const auto& it : r ){

			if( it.isValid() ){

				this->updateList( it ) ;
			}
		}
	}
}

void zuluMount::removeDisappearedEntries( const QVector< volumeEntryProperties >& entries )
{
	/*
	 * Below routine removes an entries from the table if they are found not to be
	 * present on the the list of volumes we just received.This is necessary
	 * for example to remove no longer valid options like a removed cdrom
	 */

	if( entries.isEmpty() ){

		return this->enableAll() ;
	}

	auto table = m_ui->tableWidget ;

	auto l = tablewidget::tableColumnEntries( table,0 ) ;

	auto _hasNoEntry = [&]( const QString& volume ){

		for( const auto& it : entries ){

			if( it.volumeName() == volume ){

				if( it.fileSystem() == "encfs" || it.fileSystem() == "cryfs" ){

					return false ;
				}
				if( it.volumeSize() == "Nil" ){

					return true ;
				}else{
					return false ;
				}
			}
		}

		return true ;
	} ;

	decltype( l ) z ;

	for( const auto& it : l ){

		if( _hasNoEntry( it ) ){

			z.append( it ) ;
		}
	}

	auto _done = [ this ](){

		tablewidget::selectLastRow( m_ui->tableWidget ) ;
		this->enableAll() ;
	} ;

	if( z.isEmpty() ){

		_done() ;
	}else{
		for( const auto& e : z ){

			tablewidget::selectRow( table,e ) ;
			utility::Task::suspendForOneSecond() ;
			tablewidget::deleteTableRow( table,e ) ;
		}

		utility::Task::suspendForOneSecond() ;
		_done() ;
	}
}

void zuluMount::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void zuluMount::disableAll()
{
	m_ui->pbmenu->setEnabled( false ) ;
	m_ui->pbmount->setEnabled( false ) ;
	m_ui->pbupdate->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbunlockcryptfs->setEnabled( false ) ;
}

void zuluMount::enableAll()
{
	if( m_removeAllVolumes ){

		return ;
	}
	m_ui->pbmenu->setEnabled( true ) ;
	m_ui->pbupdate->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->pbmount->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;
	m_ui->pbunlockcryptfs->setEnabled( true ) ;

	if( utility::runningInMixedMode() ){

		/*
		 * FUSE related permission issues prevents us from currently supporting
		 * unlocking encfs volumes when running in mixed mode.
		 */
		m_ui->pbunlockcryptfs->setEnabled( false ) ;
	}
}

void zuluMount::enableAll_1()
{
	m_removeAllVolumes = false ;
	this->enableAll() ;
}

#define zuluMOUNT_AUTOPATH "/.zuluCrypt/zuluMount-gui.autoMountPartitions"

bool zuluMount::autoMount()
{
	QFile f( utility::homePath() + zuluMOUNT_AUTOPATH ) ;
	return f.exists() ;
}

zuluMount::~zuluMount()
{
	QFile f( utility::homePath() + zuluMOUNT_AUTOPATH ) ;

	if( m_autoMountAction ){

		if( m_autoMountAction->isChecked() ){

			if( !f.exists() ){

				f.open( QIODevice::WriteOnly ) ;
			}
		}else{
			f.remove() ;
		}
	}

	auto q = m_ui->tableWidget ;

	const auto& r = this->window()->geometry() ;

	utility::setWindowDimensions( "zuluMount",{ r.x(),
						    r.y(),
						    r.width(),
						    r.height(),
						    q->columnWidth( 0 ),
						    q->columnWidth( 1 ),
						    q->columnWidth( 2 ),
						    q->columnWidth( 4 ),
						    q->columnWidth( 5 ) } ) ;

	delete m_ui ;
}
