/*
 *
 *  Copyright (c) 2012
 *  name : mhogo mchungu
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
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

#include <unistd.h>
#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"
#include "oneinstance.h"
#include "auto_mount.h"
#include "monitor_mountinfo.h"
#include "../zuluCrypt-gui/utility.h"
#include "task.h"

template< typename T >
class Object_raii
{
public:
	explicit Object_raii( T t ) : m_Object( t )
	{
	}
	~Object_raii()
	{
		delete m_Object ;
	}
private:
	T m_Object ;
};

#define Object_raii( x ) Object_raii< decltype( x ) > Object_raii_x( x ) ; Q_UNUSED( Object_raii_x )

MainWindow::MainWindow( int argc,char * argv[],QWidget * parent ) :QWidget( parent ),
	m_autoMountThread( 0 ),m_autoMountAction( 0 )
{
	m_argc = argc ;
	m_argv = argv ;
	this->processArgumentList() ;
}

void MainWindow::setUpApp()
{
	this->setLocalizationLanguage() ;
	m_ui = new Ui::MainWindow ;
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->tableWidget->setColumnWidth( 0,220 ) ;
	m_ui->tableWidget->setColumnWidth( 1,320 ) ;
	m_ui->tableWidget->setColumnWidth( 2,115 ) ;
	m_ui->tableWidget->setColumnWidth( 4,87 ) ;
	m_ui->tableWidget->setColumnWidth( 5,87 ) ;
	m_ui->tableWidget->hideColumn( 3 ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	m_ui->tableWidget->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents ) ;
#else
	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
#endif
	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( QIcon( QString( ":/zuluMount.png" ) ) ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( m_ui->tableWidget,SIGNAL( itemEntered( QTableWidgetItem * ) ),this,SLOT( itemEntered( QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbmount,SIGNAL( clicked() ),this,SLOT( pbMount() ) ) ;
	connect( m_ui->pbupdate,SIGNAL( clicked()),this,SLOT( pbUpdate() ) ) ;
	connect( m_ui->pbclose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_trayIcon->setIcon( QIcon( QString( ":/zuluMount.png" ) ) ) ;

	QMenu * trayMenu = new QMenu( this ) ;

	m_autoMountAction = new QAction( this ) ;
	m_autoMount = this->autoMount() ;
	m_autoMountAction->setCheckable( true ) ;
	m_autoMountAction->setChecked( m_autoMount ) ;

	m_autoMountAction->setText( tr( "automount volumes" ) ) ;

	connect( m_autoMountAction,SIGNAL( toggled( bool ) ),this,SLOT( autoMountToggled( bool ) ) ) ;

	trayMenu->addAction( m_autoMountAction ) ;

	QAction * autoOpenFolderOnMount = new QAction( this ) ;
	autoOpenFolderOnMount->setCheckable( true ) ;
	m_autoOpenFolderOnMount = this->autoOpenFolderOnMount() ;
	autoOpenFolderOnMount->setChecked( m_autoOpenFolderOnMount ) ;
	autoOpenFolderOnMount->setText( tr( "auto open mount point" ) ) ;
	connect( autoOpenFolderOnMount,SIGNAL( toggled( bool ) ),this,SLOT( autoOpenFolderOnMount( bool ) ) ) ;

	trayMenu->addAction( autoOpenFolderOnMount ) ;

	QAction * ac = new QAction( this ) ;
	ac->setText( tr( "show the interface" ) ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( raiseWindow() ) ) ;

	trayMenu->addAction( ac ) ;

	m_favorite_menu = trayMenu->addMenu( tr( "favorites" ) ) ;
	connect( m_favorite_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( favoriteClicked( QAction * ) ) ) ;
	connect( m_favorite_menu,SIGNAL( aboutToShow() ),this,SLOT( showFavorites() ) ) ;

	trayMenu->addAction( tr( "quit" ),this,SLOT( pbClose() ) ) ;
	m_trayIcon->setContextMenu( trayMenu ) ;

	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;

	m_trayIcon->show() ;

	QString dirPath = QDir::homePath() + QString( "/.zuluCrypt/" ) ;
	QDir dir( dirPath ) ;

	if( !dir.exists() ){
		dir.mkdir( dirPath ) ;
	}

	Task * t = new Task() ;

	this->disableAll() ;

	connect( t,SIGNAL( signalMountedList( QVector< volumeEntryProperties > * ) ),
		 this,SLOT( slotMountedList( QVector< volumeEntryProperties > * ) ) ) ;
	connect( t,SIGNAL( done() ),this,SLOT( openVolumeFromArgumentList() ) ) ;

	t->start( Task::Update ) ;

	this->startAutoMonitor() ;
}

void MainWindow::favoriteClicked( QAction * ac )
{
	Task * t = new Task() ;
	connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
		 this,SLOT( showMoungDialog( volumeEntryProperties * ) ) ) ;
	t->setDevice( ac->text() ) ;
	t->start( Task::VolumeType ) ;
}

void MainWindow::showFavorites()
{
	QAction * ac ;
	m_favorite_menu->clear() ;
	QStringList l = utility::readFavorites() ;
	if( l.isEmpty() ){
		ac = new QAction( tr( "list is empty" ),m_favorite_menu ) ;
		ac->setEnabled( false ) ;
		m_favorite_menu->addAction( ac ) ;
	}else{
		l.removeLast() ;
		for( const auto& it : l ){
			ac = new QAction( it.split( "\t" ).first(),m_favorite_menu ) ;
			m_favorite_menu->addAction( ac ) ;
		}
	}
}

void MainWindow::setLocalizationLanguage()
{
	QTranslator * translator = new QTranslator( this ) ;
	QString pgr      = QString( "zuluMount-gui" ) ;
	QString lang     = utility::localizationLanguage( pgr ) ;
	QString langPath = utility::localizationLanguagePath( pgr ) ;

	QByteArray r = lang.toLatin1() ;

	QByteArray e( "en_US" ) ;
	if( e == r ){
		/*
		 * english_US language,its the default and hence dont load anything
		 */
	}else{
		translator->load( r.constData(),langPath ) ;
		QCoreApplication::installTranslator( translator ) ;
	}
}

#define zuluMOUNT_AUTO_OPEN_FOLDER "/.zuluCrypt/zuluMount-gui.NoAutoOpenFolder"

void MainWindow::autoOpenFolderOnMount( bool b )
{
	QString x = QDir::homePath() + QString( zuluMOUNT_AUTO_OPEN_FOLDER ) ;

	m_autoOpenFolderOnMount = b ;
	if( b ){
		QFile::remove( x ) ;
	}else{
		QFile f( x ) ;
		f.open( QIODevice::WriteOnly ) ;
		f.close() ;
	}
}

bool MainWindow::autoOpenFolderOnMount( void )
{
	QString x = QDir::homePath() + QString( zuluMOUNT_AUTO_OPEN_FOLDER ) ;
	return QFile::exists( x ) == false ;
}

void MainWindow::startAutoMonitor()
{
	m_mountInfo = new monitor_mountinfo( this ) ;
	m_autoMountThread = new auto_mount( this ) ;

	/*
	 * perform an ordely shut down when the application terminates to prevent an occassional crash with
	 * a warning that says something like "an object was destroyed while a thread is still running"
	 *
	 * On quiting,we first shut down auto_mount object and then monitor_mountinfo object and then we
	 * close the application
	 */
	connect( m_mountInfo,SIGNAL( stopped() ),this,SLOT( quitApplication() ) ) ;
	connect( m_autoMountThread,SIGNAL( stopped() ),m_mountInfo,SLOT( stop() ) ) ;

	connect( m_mountInfo,SIGNAL( volumeRemoved( QString ) ),this,SLOT( removeEntryFromTable( QString ) ) ) ;
	connect( m_mountInfo,SIGNAL( update() ),this,SLOT( pbUpdate() ) ) ;

	m_mountInfo->start() ;
	m_autoMountThread->start() ;
}

/*
 * This should be the only function that closes the application
 */
void MainWindow::pbClose()
{
	m_autoMountThread->stop() ;
}

void MainWindow::quitApplication()
{
	QCoreApplication::quit() ;
}

void MainWindow::autoMountVolume( volumeEntryProperties * entry )
{
	Object_raii( entry ) ;

	QStringList l = entry->entryList() ;
	if( entry->entryisValid() ){
		if( entry->encryptedVolume() ){
			this->addEntryToTable( true,l ) ;
		}else{
			if( m_autoMount ){
				mountPartition * mp = new mountPartition( this,m_ui->tableWidget,
									  m_folderOpener,m_autoOpenFolderOnMount ) ;
				connect( mp,SIGNAL( autoMountComplete() ),mp,SLOT( deleteLater() ) ) ;
				mp->AutoMount( l ) ;
			}else{
				this->addEntryToTable( false,l ) ;
			}
		}
	}
}

void MainWindow::deviceRemoved( QString dev )
{
	QTableWidget * table = m_ui->tableWidget ;
	int row = tablewidget::columnHasEntry( table,dev ) ;
	if( row != -1 ){
		tablewidget::deleteRowFromTable( table,row ) ;
		/*
		* see if a user just removed the device without properly closing it/unmounting it
		* and try to do so for them
		*/
		Task * t = new Task() ;
		t->setDevice( dev ) ;
		t->start( Task::checkUnMount ) ;
		this->enableAll() ;
	}
}

void MainWindow::itemEntered( QTableWidgetItem * item )
{
	int row = item->row() ;
	QTableWidget * table = item->tableWidget() ;
	QString m_point = table->item( row,1 )->text() ;

	QString x = table->item( row,3 )->text() ;
	QString z ;
	QString y ;

	if( m_point == QString( "/" ) ){
		/*
		 * we dont check if root path is publicly shared because the path it will produce (/run/media/shared/)
		 * will always return true,a solution is to examine /proc/self/mountinfo and thats work for another day
		 */
		if( x == QString( "Nil" ) ){
			x.clear() ;
		}
		z += tr( "LABEL=\"%1\"" ).arg( x ) ;

	}else if( m_point == QString( "Nil" ) ){
		/*
		 * volume is not mounted,cant know its LABEL value
		 */
		x.clear() ;
		z += tr( "LABEL=\"%1\"" ).arg( x ) ;
	}else{
		if( x == QString( "Nil" ) ){
			x.clear() ;
		}
		y = utility::shareMountPointToolTip( m_point ) ;
		if( y.isEmpty() ){
			z += tr( "LABEL=\"%1\"" ).arg( x ) ;
		}else{
			z += tr( "LABEL=\"%1\"\n%2" ).arg( x ).arg( y ) ;
		}
	}
	item->setToolTip( z ) ;
}

void MainWindow::processArgumentList()
{
	QStringList l = QCoreApplication::arguments() ;

	m_startHidden  = l.contains( "-e" ) ;
	m_device       = utility::cmdArgumentValue( l,"-d" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;
}

void MainWindow::startGUI()
{
	if( !m_startHidden ){
		this->raiseWindow() ;
	}
}

void MainWindow::raiseWindow()
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
}

void MainWindow::raiseWindow( QString device )
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
	m_device = device ;
	this->openVolumeFromArgumentList() ;
}

void MainWindow::start()
{
	QString sockpath = QString( "zuluMount-gui.socket" ) ;
	oneinstance * instance = new oneinstance( this,sockpath,"startGUI",m_device ) ;
	if( !instance->instanceExist() ){
		connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;
		connect( instance,SIGNAL( raiseWithDevice( QString ) ),this,SLOT( raiseWindow( QString ) ) ) ;
		this->setUpApp() ;
	}
}

void MainWindow::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() ) ;

	QString mt = m_ui->tableWidget->item( item->row(),1 )->text() ;
	QString type = m_ui->tableWidget->item( item->row(),2 )->text() ;

	if( mt == QString( "Nil" ) ){
		connect( m.addAction( tr( "mount" ) ),SIGNAL( triggered() ),this,SLOT( slotMount() ) ) ;
	}else{
		QString mp = QString( "/run/media/private/%1/" ).arg( utility::userName() ) ;
		QString mp_1 = QString( "/home/%1/" ).arg( utility::userName() ) ;
		if( mt.startsWith( mp ) || mt.startsWith( mp_1 ) ){
			connect( m.addAction( tr( "unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
			m.addSeparator() ;
			connect( m.addAction( tr( "properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
			m.addSeparator() ;
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;
			if( m_sharedFolderPath.isEmpty() ){
				connect( m.addAction( tr( "open folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenFolder() ) ) ;
			}else{
				connect( m.addAction( tr( "open private folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenFolder() ) ) ;
				connect( m.addAction( tr( "open shared folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}else{
			m_sharedFolderPath = utility::sharedMountPointPath( mt ) ;
			if( m_sharedFolderPath.isEmpty() ){
				if( utility::pathIsReadable( mt ) ){
					connect( m.addAction( tr( "properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
					m.addSeparator() ;
					connect( m.addAction( tr( "open folder" ) ),SIGNAL( triggered() ),
						 this,SLOT( slotOpenFolder() ) ) ;
				}else{
					connect( m.addAction( tr( "properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
				}
			}else{
				connect( m.addAction( tr( "properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
				m.addSeparator() ;
				connect( m.addAction( tr( "open shared folder" ) ),SIGNAL( triggered() ),
					 this,SLOT( slotOpenSharedFolder() ) ) ;
			}
		}
	}

	m.addSeparator() ;
	m.addAction( tr( "close menu" ) ) ;

	if( itemClicked ){
		m.exec( QCursor::pos() ) ;
	}else{
		QPoint p = this->pos() ;
		int x = p.x() + 100 + m_ui->tableWidget->columnWidth( 0 ) ;
		int y = p.y() + 50 + m_ui->tableWidget->rowHeight( 0 ) * item->row() ;
		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

void MainWindow::itemClicked( QTableWidgetItem * item )
{
	this->showContextMenu( item,true ) ;
}

void MainWindow::defaultButton()
{
	int row = m_ui->tableWidget->currentRow() ;
	QString mt = m_ui->tableWidget->item( row,1 )->text() ;

	if( mt == QString( "Nil" ) ){
		this->slotMount() ;
	}else{
		this->showContextMenu( m_ui->tableWidget->currentItem(),false ) ;
	}
}

void MainWindow::fileManagerOpenStatus( int exitCode, int exitStatus,int startError )
{
	QString x = tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) ;
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),x ) ;
	}
}

void MainWindow::slotOpenSharedFolder()
{
	Task * t = new Task() ;
	t->setMountPoint( m_sharedFolderPath ) ;
	t->setMountPointOpener( m_folderOpener ) ;
	connect( t,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	t->start( Task::openMountPoint ) ;
}

void MainWindow::slotOpenFolder()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString path = m_ui->tableWidget->item( item->row(),1 )->text() ;

	Task * t = new Task() ;
	t->setMountPoint( path ) ;
	t->setMountPointOpener( m_folderOpener ) ;
	connect( t,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	t->start( Task::openMountPoint ) ;
}

void MainWindow::volumeProperties()
{
	this->disableAll() ;

	Task * t = new Task() ;
	t->setDevice( m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),0 )->text() ) ;
	t->setType( m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),2 )->text() ) ;
	connect( t,SIGNAL( signalProperties( QString ) ),this,SLOT( volumeProperties( QString ) ) ) ;

	t->start( Task::VolumeProperties ) ;
}

void MainWindow::volumeProperties( QString properties )
{
	DialogMsg msg( this ) ;

	if( properties.isEmpty() ){
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
	}else{
		int i = properties.indexOf( "\n" ) ;
		if( i != -1 ){
			msg.ShowUIVolumeProperties( tr( "volume properties" ),properties.mid( i + 1 ) ) ;
		}else{
			msg.ShowUIOK( tr( "ERROR" ),
				      tr( "could not get volume properties.\nvolume is not open or was opened by a different user" ) ) ;
		}
	}
	this->enableAll() ;
}

void MainWindow::setUpShortCuts()
{
	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter ) ;
	keys.append( Qt::Key_Return ) ;
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac ) ;

	QAction * qa = new QAction( this ) ;
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
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbClose() ) ) ;
	this->addAction( qa ) ;
}

void MainWindow::setUpFont()
{
	userfont F( this ) ;
	this->setFont( F.getFont() ) ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}

void MainWindow::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){
		if( this->isVisible() ){
			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void MainWindow::autoMountToggled( bool opt )
{
	m_autoMount = opt ;
}

void MainWindow::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void MainWindow::dropEvent( QDropEvent * e )
{
	const QMimeData * m = e->mimeData() ;
	QList<QUrl> l = m->urls() ;

	for( const auto& it : l ){
		m_device = it.path() ;
		if( utility::pathPointsToAFile( m_device ) ){
			Task * t = new Task() ;
			connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
				 this,SLOT( showMoungDialog( volumeEntryProperties * ) ) ) ;
			t->setDevice( m_device ) ;
			t->start( Task::VolumeType ) ;
		}
	}
}

void MainWindow::mount( const volumeEntryProperties& entry )
{
	this->disableAll() ;
	if( entry.encryptedVolume() ){
		keyDialog * kd = new keyDialog( this,m_ui->tableWidget,entry.volumeName(),
						entry.fileSystem(),m_folderOpener,m_autoOpenFolderOnMount ) ;
		connect( kd,SIGNAL( cancel() ),this,SLOT( enableAll() ) ) ;
		kd->ShowUI() ;
	}else{
		mountPartition * mp = new mountPartition( this,m_ui->tableWidget,m_folderOpener,m_autoOpenFolderOnMount ) ;
		connect( mp,SIGNAL( cancel() ),this,SLOT( enableAll() ) ) ;
		mp->ShowUI( entry.volumeName(),entry.label() ) ;
	}
}

void MainWindow::openVolumeFromArgumentList()
{
	if( !m_device.isEmpty() ){
		Task * t = new Task() ;
		connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
			 this,SLOT( showMoungDialog( volumeEntryProperties * ) ) ) ;
		t->setDevice( m_device ) ;
		t->start( Task::VolumeType ) ;
	}
}

void MainWindow::slotMount()
{
	QTableWidget * table = m_ui->tableWidget ;
	int row = table->currentRow() ;

	QStringList l ;
	l.append( table->item( row,0 )->text() ) ;
	l.append( table->item( row,1 )->text() ) ;
	l.append( table->item( row,2 )->text() ) ;
	l.append( table->item( row,3 )->text() ) ;
	l.append( table->item( row,4 )->text() ) ;
	l.append( table->item( row,5 )->text() ) ;

	volumeEntryProperties entry( l ) ;

	this->mount( entry ) ;
}

void MainWindow::showMoungDialog( volumeEntryProperties * entry )
{
	Object_raii( entry ) ;

	if( entry ){
		this->mount( *entry ) ;
	}else{
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),
			      tr( "permission to access the volume was denied\nor\nthe volume is not supported\n(LVM/MDRAID signatures found)" ) ) ;
		this->enableAll() ;
	}
}

void MainWindow::pbMount()
{
	this->disableAll() ;

	QString path = QFileDialog::getOpenFileName( this,tr( "select an image file to mount" ),QDir::homePath() ) ;
	if( path.isEmpty() ){
		this->enableAll() ;
	}else{
		m_device = path ;
		Task * t = new Task() ;
		connect( t,SIGNAL( volumeMiniProperties( volumeEntryProperties * ) ),
			 this,SLOT( showMoungDialog( volumeEntryProperties * ) ) ) ;
		t->setDevice( m_device ) ;
		t->start( Task::VolumeType ) ;
	}
}

QFont MainWindow::getSystemVolumeFont()
{
	QFont f = this->font() ;
	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;
	return f ;
}

void MainWindow::addEntryToTable( bool systemVolume,const QStringList& l )
{
	if( systemVolume ){
		tablewidget::addRowToTable( m_ui->tableWidget,l,this->getSystemVolumeFont() ) ;
	}else{
		tablewidget::addRowToTable( m_ui->tableWidget,l ) ;
	}
}

void MainWindow::removeEntryFromTable( QString volume )
{
	QTableWidget * table = m_ui->tableWidget ;

	int r = tablewidget::columnHasEntry( table,volume ) ;
	if( r != -1 ){
		tablewidget::deleteRowFromTable( table,r ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void MainWindow::volumeMiniProperties( volumeEntryProperties * volumeInfo )
{
	Object_raii( volumeInfo ) ;

	this->disableAll() ;

	if( volumeInfo ){
		this->updateList( *volumeInfo ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void MainWindow::updateList( const volumeEntryProperties& entry )
{
	QTableWidget * table = m_ui->tableWidget ;

	int row = tablewidget::columnHasEntry( table,entry.volumeName() ) ;
	if( row == -1 ){
		/*
		 * volume has no entry in the list probably because its a volume based on a file.
		 * Add an entry to the list to accomodate it
		 */
		row = tablewidget::addEmptyRow( table ) ;
	}

	tablewidget::setText( table,row,0,entry.volumeName() ) ;
	tablewidget::setText( table,row,1,entry.mountPoint() ) ;
	tablewidget::setText( table,row,2,entry.fileSystem() ) ;
	tablewidget::setText( table,row,3,entry.label() ) ;
	tablewidget::setText( table,row,4,entry.volumeSize() ) ;
	tablewidget::setText( table,row,5,entry.spaceUsedPercentage() ) ;

	if( entry.isSystem() ){
		tablewidget::setRowFont( table,row,this->getSystemVolumeFont() ) ;
	}else{
		tablewidget::setRowFont( table,row,this->font() ) ;
	}
	tablewidget::selectRow( table,row ) ;
}

void MainWindow::pbUmount()
{
	this->disableAll() ;

	int row = m_ui->tableWidget->currentRow() ;

	QString path = m_ui->tableWidget->item( row,0 )->text() ;
	QString type = m_ui->tableWidget->item( row,2 )->text() ;

	Task * t = new Task() ;

	t->setDevice( path ) ;
	t->setType( type ) ;

	connect( t,SIGNAL( signalUnmountComplete( int,QString) ),this,SLOT( slotUnmountComplete( int,QString ) ) ) ;

	t->start( Task::Unmount ) ;
}

void MainWindow::pbUpdate()
{
	this->disableAll() ;

	m_ui->tableWidget->setEnabled( false ) ;

	QTableWidget * table = m_ui->tableWidget ;
	while( table->rowCount() > 0 ){
		table->removeRow( 0 ) ;
	}
	Task * t = new Task() ;
	connect( t,SIGNAL( signalMountedList( QVector< volumeEntryProperties > * ) ),
		 this,SLOT( slotUpdateMountedList( QVector< volumeEntryProperties > * ) ) ) ;
	t->start( Task::Update ) ;
}

void MainWindow::errorReadingList()
{
	DialogMsg msg( this ) ;
	msg.ShowUIOK( tr( "ERROR" ),
		      tr( "reading partition properties took longer than expected and operation was terminated,click refresh to try again" ) ) ;
	this->enableAll() ;
}

void MainWindow::slotUpdateMountedList( QVector< volumeEntryProperties > * entries )
{
	Object_raii( entries ) ;

	if( entries->isEmpty() ){
		this->errorReadingList() ;
	}else{
		for( const auto& it : *entries ){

			if( it.entryisValid() ){
				this->updateList( it ) ;
			}
		}

		tablewidget::selectLastRow( m_ui->tableWidget ) ;
		this->enableAll() ;
	}
}

void MainWindow::nnggrrr( QVector< volumeEntryProperties > * entries )
{
	/*
	 * broken and not used
	 */
	/*
	 * Below routine removes an entry on the table if it is found not to be
	 * present on the the list of volumes we just received.This is necessary
	 * for example to remove no longer valid options like a removed cdrom
	 */

	QTableWidget * table = m_ui->tableWidget ;

	QStringList l = tablewidget::tableEntries( table ) ;

	auto _hasNoEntry = [&]( const QString& volume ){
		for( const auto& it : *entries ){
			if( it.volumeName() == volume ){
				return false ;
			}
		}

		return true ;
	} ;

	for( const auto& it : l ){
		if( _hasNoEntry( it ) ){
			tablewidget::deleteTableRow( table,it ) ;
		}
	}
}

void MainWindow::slotMountedList( QVector< volumeEntryProperties > * entries )
{
	Object_raii( entries ) ;

	if( entries->isEmpty() ){
		return this->errorReadingList() ;
	}

	QTableWidget * table = m_ui->tableWidget ;

	QFont f = this->font() ;

	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;

	QStringList l ;

	for( const auto& it : *entries ){

		if( it.entryisValid() ){

			l.clear() ;
			l.append( it.volumeName() ) ;
			l.append( it.mountPoint() ) ;
			l.append( it.fileSystem() ) ;
			l.append( it.label() ) ;
			l.append( it.volumeSize() ) ;
			l.append( it.spaceUsedPercentage() ) ;

			if( it.isSystem() ){
				tablewidget::addRowToTable( table,l,f ) ;
			}else{
				tablewidget::addRowToTable( table,l ) ;
			}
		}
	}

	this->enableAll() ;
}

void MainWindow::slotUnmountComplete( int status,QString msg )
{
	if( status ){
		DialogMsg m( this ) ;
		m.ShowUIOK( tr( "ERROR" ),msg ) ;
		this->enableAll() ;
	}
}

void MainWindow::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void MainWindow::disableAll()
{
	m_ui->pbclose->setEnabled( false ) ;
	m_ui->pbmount->setEnabled( false ) ;
	m_ui->pbupdate->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
}

void MainWindow::enableAll()
{
	m_ui->pbclose->setEnabled( true ) ;
	m_ui->pbupdate->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
	m_ui->pbmount->setEnabled( true ) ;
	m_ui->tableWidget->setFocus() ;
}

#define zuluMOUNT_AUTOPATH "/.zuluCrypt/zuluMount-gui.autoMountPartitions"

bool MainWindow::autoMount()
{
	QFile f( QDir::homePath() + QString( zuluMOUNT_AUTOPATH ) ) ;
	return f.exists() ;
}

MainWindow::~MainWindow()
{
	QFile f( QDir::homePath() + QString( zuluMOUNT_AUTOPATH ) ) ;

	if( m_autoMountAction ){
		if( m_autoMountAction->isChecked() ){
			if( !f.exists() ){
				f.open( QIODevice::WriteOnly ) ;
				f.close() ;
			}
		}else{
			f.remove() ;
		}
	}
}
