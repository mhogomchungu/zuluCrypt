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
MainWindow::MainWindow( QWidget * parent ) :QWidget( parent )
{
}

void MainWindow::setUpApp()
{
	m_ui = new Ui::MainWindow ;
	m_ui->setupUi(this);

	this->setFixedSize( this->size() ) ;

	m_ui->tableWidget->setColumnWidth( 0,228 );
	m_ui->tableWidget->setColumnWidth( 1,226 );
	m_ui->tableWidget->setColumnWidth( 2,98 );
	m_ui->tableWidget->setColumnWidth( 3,98 );
	m_ui->tableWidget->setColumnWidth( 4,87 );
	m_ui->tableWidget->setColumnWidth( 5,87 );

	this->setWindowIcon( QIcon( QString( ":/zuluMount.png" ) ) );

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->pbmount,SIGNAL( clicked() ),this,SLOT( pbMount() ) ) ;
	connect( m_ui->pbunmount,SIGNAL( clicked() ),this,SLOT( pbUmount() ) ) ;
	connect( m_ui->pbupdate,SIGNAL( clicked()),this,SLOT(pbUpdate() ) ) ;
	connect( m_ui->pbclose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->cbReadOnly,SIGNAL( stateChanged( int ) ),this,SLOT( stateChanged( int ) ) ) ;

	m_ui->cbReadOnly->setVisible( false );

	this->setUpShortCuts();

	this->setUpFont();

	m_trayIcon = new QSystemTrayIcon( this ) ;
	m_trayIcon->setIcon( QIcon( QString( ":/zuluMount.png" ) ) );

	QMenu * trayMenu = new QMenu( this ) ;
	trayMenu->addAction( tr( "quit" ),this,SLOT( slotCloseApplication() ) );
	m_trayIcon->setContextMenu( trayMenu );

	connect( m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) );

	managepartitionthread * part = new managepartitionthread() ;

	this->disableAll();

	connect( part,SIGNAL( signalMountedList( QStringList,QStringList ) ),this,SLOT( slotMountedList( QStringList,QStringList ) ) ) ;

	part->startAction( QString( "update" ) ) ;

	m_working = false ;
	m_trayIcon->show();

	QString dirPath = QDir::homePath() + QString( "/.zuluCrypt/" ) ;
	QDir dir( dirPath ) ;

	if( !dir.exists() )
		dir.mkdir( dirPath ) ;

	m_ui->pbunmount->setVisible( false );

	QStringList argv = QCoreApplication::arguments() ;
	if( argv.size() < 2 ){
		m_folderOpener = QString( "xdg-open" ) ;
	}else{
		m_folderOpener = argv.at( 1 ) ;
	}

	this->show();
}

void MainWindow::defaultButton()
{
	int row = m_ui->tableWidget->currentRow() ;
	QString mt = m_ui->tableWidget->item( row,1 )->text() ;

	if( mt == QString( "Nil" ) )
		this->slotMount();
	else
		this->pbUmount();
}

void MainWindow::raiseWindow()
{
	this->setVisible( true );
	this->raise();
	this->show();
	this->setWindowState( Qt::WindowActive ) ;
}

void MainWindow::start()
{
	QString sockpath = QString( "zuluMount-gui.socket" ) ;
	oneinstance * instance = new oneinstance( this,sockpath,"raiseWindow" ) ;
	connect( instance,SIGNAL( raise() ),this,SLOT( raiseWindow() ) ) ;

	if( !instance->instanceExist() )
		this->setUpApp();
}

void MainWindow::pbClose()
{
	this->slotCloseApplication();
}

void MainWindow::slotCloseApplication()
{
	if( m_working == false )
		QCoreApplication::quit();
}

void MainWindow::itemClicked( QTableWidgetItem * item )
{
	QMenu m ;

	m.setFont( this->font() );

	QString mt = m_ui->tableWidget->item( item->row(),1 )->text() ;
	QString type = m_ui->tableWidget->item( item->row(),2 )->text() ;

	if( mt == QString( "Nil" ) ){
		connect( m.addAction( tr( "mount" ) ),SIGNAL( triggered() ),this,SLOT( slotMount() ) ) ;
	}else{
		connect( m.addAction( tr( "unmount" ) ),SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;

		if( type.contains( QString( "crypto" ) ) ) {
			m.addSeparator() ;
			connect( m.addAction( tr( "properties" ) ),SIGNAL( triggered() ),this,SLOT( volumeProperties() ) ) ;
		}
		m.addSeparator() ;
		connect( m.addAction( tr( "open folder" ) ),SIGNAL( triggered() ),this,SLOT( slotOpenFolder() ) ) ;
	}

	m.exec( QCursor::pos() ) ;
}

void MainWindow::fileManagerOpenStatus( int exitCode, int exitStatus,int startError )
{
	Q_UNUSED( startError ) ;
	if( exitCode != 0 || exitStatus != 0 ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "warning" ),tr( "could not open mount point because \"%1\" tool does not appear to be working correctly").arg( m_folderOpener ) );
	}
}

void MainWindow::slotOpenFolder()
{
	QTableWidgetItem * item = m_ui->tableWidget->currentItem() ;
	QString path = m_ui->tableWidget->item( item->row(),1 )->text() ;
	openmountpointinfilemanager * ofm = new openmountpointinfilemanager( m_folderOpener,path ) ;
	connect( ofm,SIGNAL( errorStatus( int,int,int ) ),this,SLOT( fileManagerOpenStatus( int,int,int ) ) ) ;
	ofm->start();
}

void MainWindow::volumeProperties()
{
	this->disableAll();
	managepartitionthread * part = new managepartitionthread() ;
	part->setDevice( m_ui->tableWidget->item( m_ui->tableWidget->currentRow(),0 )->text() );
	connect( part,SIGNAL( signalProperties( QString ) ),this,SLOT( volumeProperties( QString ) ) ) ;

	part->startAction( QString( "volumeProperties" ) ) ;
}

void MainWindow::volumeProperties( QString properties )
{
	DialogMsg msg( this ) ;

	if( properties.isEmpty() ){
		msg.ShowUIOK( tr("ERROR"),tr("could not get volume properties" ) ) ;
	}else{
		int i = properties.indexOf( "\n" ) ;
		if( i != -1 ){
			msg.ShowUIVolumeProperties( tr("volume properties" ),properties.mid( i + 1 ) ) ;
		}else{
			msg.ShowUIOK( tr("ERROR"),tr("could not get volume properties" ) ) ;
		}
	}
	this->enableAll();
}

void MainWindow::setUpShortCuts()
{
	QAction * ac = new QAction( this ) ;
	QList<QKeySequence> keys ;
	keys.append( Qt::Key_Enter );
	keys.append( Qt::Key_Return );
	ac->setShortcuts( keys ) ;
	connect( ac,SIGNAL( triggered() ),this,SLOT( defaultButton() ) ) ;
	this->addAction( ac );

	QAction * qa = new QAction( this ) ;
	QList<QKeySequence> z ;
	z.append( Qt::Key_M );
	qa->setShortcuts( z ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbMount() ) ) ;
	this->addAction( qa );

	qa = new QAction( this ) ;
	QList<QKeySequence> p ;
	p.append( Qt::Key_U );
	qa->setShortcuts( p ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUmount() ) ) ;
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> q ;
	q.append( Qt::Key_R );
	qa->setShortcuts( q ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( pbUpdate() ) );
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> d ;
	d.append( Qt::Key_O );
	qa->setShortcuts( d ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( slotcbReadOnly() ) );
	this->addAction( qa ) ;

	qa = new QAction( this ) ;
	QList<QKeySequence> e ;
	e.append( Qt::Key_C );
	qa->setShortcuts( e ) ;
	connect( qa,SIGNAL( triggered() ),this,SLOT( slotCloseApplication() ) );
	this->addAction( qa ) ;
}

void MainWindow::setUpFont()
{
	userfont F( this ) ;
	this->setFont( F.getFont() ) ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->hide();
}

void MainWindow::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){
		if( this->isVisible() )
			this->hide();
		else
			this->show();
	}
}

void MainWindow::slotcbReadOnly()
{
	if( m_ui->cbReadOnly->isChecked() )
		m_ui->cbReadOnly->setChecked( false );
	else
		m_ui->cbReadOnly->setChecked( true );
}

void MainWindow::stateChanged( int state )
{
	m_ui->cbReadOnly->setEnabled( false );
	m_ui->cbReadOnly->setChecked( openvolumereadonly::setOption( this,state,QString( "zuluMount-gui" ) ) );
	m_ui->cbReadOnly->setEnabled( true );
}

void MainWindow::mount( QString type,QString device,QString label )
{
	if( type == QString( "crypto_LUKS" ) || type == QString( "Nil" ) ){
		keyDialog * kd = new keyDialog( this,m_ui->tableWidget,device,type,m_folderOpener ) ;
		connect( kd,SIGNAL( hideUISignal() ),kd,SLOT( deleteLater() ) ) ;
		connect( kd,SIGNAL( hideUISignal() ),this,SLOT( enableAll() ) ) ;
		kd->ShowUI();
	}else{
		mountPartition * mp = new mountPartition( this,m_ui->tableWidget,m_folderOpener ) ;
		connect( mp,SIGNAL( hideUISignal() ),mp,SLOT( deleteLater() ) ) ;
		connect( mp,SIGNAL( hideUISignal() ),this,SLOT( enableAll() ) ) ;
		mp->ShowUI( device,label );
	}
}

void MainWindow::slotMount()
{
	QTableWidget * table = m_ui->tableWidget ;
	int row = table->currentRow() ;
	QString device = table->item( row,0 )->text() ;
	QString type   = table->item( row,2 )->text() ;
	QString label  = table->item( row,3 )->text() ;
	this->mount( type,device,label );
}

void MainWindow::pbMount()
{
	this->disableAll();

	QString path = QFileDialog::getOpenFileName( this,tr( "select an image file to mount" ),QDir::homePath() ) ;
	if( path.isEmpty() ){
		this->enableAll();
		return ;
	}
	QStringList prp = utility::deviceProperties( path ) ;

	QString type = prp.at( 3 ) ;
	QString label = prp.at( 2 ) ;

	m_device = path ;
	this->mount( type,m_device,label );
}

void MainWindow::volumeMiniProperties( QTableWidget * table,QString p,QString mountPointPath )
{
	QStringList l ;
	QString device ;
	QString fileSystem ;
	QString total ;
	QString perc ;
	QString label ;

	if( p.isEmpty() ){
		device = QString( "Nil" ) ;
		fileSystem = QString( "Nil" ) ;
		total = QString( "0" ) ;
		perc  = QString( "0%" );
		label = QString( "Nil" ) ;
	}else{
		l = p.split( "\t" ) ;
		device = l.at( 0 ) ;
		fileSystem = l.at( 2 ) ;
		label = l.at( 3 ) ;
		total = l.at( 4 ) ;
		perc = l.at( 5 ) ;
		perc.remove( QChar( '\n' ) ) ;
	}

	if( !device.startsWith( QString( "/dev/" ) ) ){
		tablewidget::addEmptyRow( table ) ;
	}

	int row = table->currentRow() ;
	tablewidget::setText( table,row,0,device ) ;
	tablewidget::setText( table,row,1,mountPointPath ) ;
	tablewidget::setText( table,row,2,fileSystem ) ;
	tablewidget::setText( table,row,3,label ) ;
	tablewidget::setText( table,row,4,total ) ;
	tablewidget::setText( table,row,5,perc ) ;
}

void MainWindow::pbUmount()
{
	this->disableAll();

	int row = m_ui->tableWidget->currentRow() ;

	QString path = m_ui->tableWidget->item( row,0 )->text() ;
	QString type = m_ui->tableWidget->item( row,2 )->text() ;

	managepartitionthread * part = new managepartitionthread() ;

	part->setDevice( path );
	part->setType( type );

	connect( part,SIGNAL( signalUnmountComplete( int,QString ) ),this,SLOT( slotUnmountComplete( int,QString ) ) ) ;

	part->startAction( QString( "umount" ) ) ;
}

void MainWindow::pbUpdate()
{
	this->disableAll();

	while( m_ui->tableWidget->rowCount() )
		m_ui->tableWidget->removeRow( 0 ) ;

	managepartitionthread * part = new managepartitionthread() ;

	m_ui->tableWidget->setEnabled( false );
	connect( part,SIGNAL( signalMountedList( QStringList,QStringList ) ),this,SLOT( slotMountedList( QStringList,QStringList ) ) ) ;

	part->startAction( QString( "update" ) ) ;
}

void MainWindow::slotMountedList( QStringList list,QStringList sys )
{
	if( list.isEmpty() || sys.isEmpty() ){
		DialogMsg msg( this ) ;
		msg.ShowUIOK( tr( "ERROR" ),tr( "reading partition properties took longer than expected and operation was terminated,click refresh to try again" ) );
		this->enableAll();
		return ;
	}

	QTableWidget * table = m_ui->tableWidget ;

	QStringList entries ;

	int j = list.size() - 1 ;

	QFont f = this->font() ;

	f.setItalic( !f.italic() );
	f.setBold( !f.bold() );

	QString opt ;
	QString fs ;
	for( int i = 0 ; i < j ; i++ ){
		entries = list.at( i ).split( '\t' ) ;
		if( entries.size() < 6 ){
			continue ;
		}

		fs =  entries.at( 2 ) ;
		if( fs == QString( "swap" ) || fs.contains( QString( "LVM" ) ) || fs.contains( QString( "lvm" ) ) || fs.contains( QString( "mdraid" ) ) ){
			continue ;
		}
		opt = entries.at( 4 ) ;
		if( opt == QString( "Nil" ) || opt == QString( "1.0 KB" ) )
			continue ;
		if( sys.contains( entries.at( 0 ) ) )
			tablewidget::addRowToTable( table,entries,f ) ;
		else
			tablewidget::addRowToTable( table,entries ) ;
	}

	if( m_ui->tableWidget->rowCount() > 10 )
		m_ui->tableWidget->setColumnWidth( 1,226 );
	else
		m_ui->tableWidget->setColumnWidth( 1,240 );

	this->enableAll();
}

void MainWindow::slotUnmountComplete( int status,QString msg )
{
	if( status ){
		DialogMsg m( this ) ;
		m.ShowUIOK( QString( "ERROR" ),msg );
	}else{
		QTableWidget * table = m_ui->tableWidget ;

		int row = table->currentRow() ;

		QString type = table->item( row,2 )->text() ;
		QString device = table->item( row,0 )->text() ;

		if( device.startsWith( QString( "/dev/" ) ) || device.startsWith( "UUID=" ) ){
			table->item( row,1 )->setText( QString( "Nil" ) );

			if( type == QString( "crypto_LUKS" ) )
				table->item( row,3 )->setText( QString( "Nil" ) );
			else if( type == QString( "crypto_PLAIN" ) ){
				table->item( row,3 )->setText( QString( "Nil" ) );
				table->item( row,2 )->setText( QString( "Nil" ) );
			}else if( type == QString( "crypto_TCRYPT" ) ){
				table->item( row,3 )->setText( QString( "Nil" ) );
				table->item( row,2 )->setText( QString( "Nil" ) );
			}
			table->item( row,5 )->setText( QString( "Nil" ) );
		}else{
			tablewidget::deleteRowFromTable( m_ui->tableWidget,row ) ;
		}
	}

	this->enableAll();
}

void MainWindow::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectTableRow( current,previous ) ;
}

void MainWindow::disableAll()
{
	m_ui->cbReadOnly->setEnabled( false );
	m_ui->pbclose->setEnabled( false );
	m_ui->pbmount->setEnabled( false );
	m_ui->pbupdate->setEnabled( false );
	m_ui->tableWidget->setEnabled( false );
	m_ui->pbunmount->setEnabled( false );
	m_working = true ;
}

void MainWindow::enableAll()
{
	m_ui->cbReadOnly->setEnabled( true );
	m_ui->pbclose->setEnabled( true );
	m_ui->pbupdate->setEnabled( true );
	m_ui->tableWidget->setEnabled( true );
	m_working = false ;
	m_ui->pbmount->setEnabled( true );
	m_ui->tableWidget->setFocus();
}

MainWindow::~MainWindow()
{
}
