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

#include "deviceoffset.h"
#include "mountpartition.h"
#include "ui_mountpartition.h"
#include <QDebug>

#include <QFile>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QCloseEvent>
#include <QByteArray>
#include <QFileDialog>
#include <QFont>
#include <QTableWidget>
#include <QAction>
#include <QMenu>
#include <QCursor>
#include <QAction>

#include "zulumounttask.h"
#include "bin_path.h"
#include "task.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "../zuluCrypt-gui/utility.h"
#include "mountoptions.h"

mountPartition::mountPartition( QWidget * parent,QTableWidget * table,std::function< void() > p,std::function< void( const QString& ) > q ) :
	QWidget( parent ),m_ui( new Ui::mountPartition ),m_cancel( std::move( p ) ),m_success( std::move( q ) )
{
	m_ui->setupUi( this ) ;
	m_ui->checkBoxShareMountPoint->setToolTip( utility::shareMountPointToolTip() ) ;
	m_table = table ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	m_ui->pbMount->setFocus() ;

	m_ui->checkBoxMountReadOnly->setChecked( utility::getOpenVolumeReadOnlyOption( "zuluMount-gui" ) ) ;

	connect( m_ui->pbOptions,SIGNAL( clicked() ),this,SLOT( pbOptions() ) ) ;
	connect( m_ui->pbMount,SIGNAL( clicked() ),this,SLOT( pbMount() ) ) ;
	connect( m_ui->pbMountFolder,SIGNAL( clicked() ),this,SLOT( pbOpenMountPath() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->checkBox,SIGNAL( stateChanged( int ) ),this,SLOT( stateChanged( int ) ) ) ;
	connect( m_ui->checkBoxMountReadOnly,SIGNAL( stateChanged(int) ),this,SLOT( checkBoxReadOnlyStateChanged( int ) ) ) ;

	m_ui->pbMountFolder->setIcon( QIcon( ":/folder.png" ) ) ;

	this->setFont( parent->font() ) ;

	m_ui->pbMountFolder->setVisible( false ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcut( Qt::CTRL + Qt::Key_F ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( showOffSetWindowOption() ) ) ;

		return ac ;
	}() ) ;

	m_menu = new QMenu( this ) ;

	m_menu->addAction( tr( "Set File System Options" ) ) ;
	m_menu->addAction( tr( "Set Volume Offset" ) ) ;

	m_menu->setFont( this->font() ) ;

	connect( m_menu,SIGNAL( triggered( QAction * ) ),this,SLOT( doAction( QAction * ) ) ) ;

	this->installEventFilter( this ) ;
}

bool mountPartition::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void mountPartition::checkBoxReadOnlyStateChanged( int state )
{
	m_ui->checkBoxMountReadOnly->setEnabled( false ) ;
	m_ui->checkBoxMountReadOnly->setChecked( utility::setOpenVolumeReadOnly( this,state == Qt::Checked,"zuluMount-gui" ) ) ;

	m_ui->checkBoxMountReadOnly->setEnabled( true ) ;
	if( m_ui->lineEdit->text().isEmpty() ){
		m_ui->lineEdit->setFocus() ;
	}else{
		m_ui->pbMount->setFocus() ;
	}
}

void mountPartition::enableAll()
{
	if( m_label != "Nil" ){
		m_ui->checkBox->setEnabled( true ) ;
	}
	m_ui->checkBoxMountReadOnly->setEnabled( true ) ;
	m_ui->checkBoxShareMountPoint->setEnabled( true ) ;
	m_ui->labelMountPoint->setEnabled( true ) ;
	m_ui->lineEdit->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbMount->setEnabled( true ) ;
	m_ui->pbMountFolder->setEnabled( true ) ;
	m_ui->pbOptions->setEnabled( true ) ;
}

void mountPartition::disableAll()
{
	m_ui->pbMount->setEnabled( false ) ;
	m_ui->checkBox->setEnabled( false ) ;
	m_ui->checkBoxMountReadOnly->setEnabled( false ) ;
	m_ui->checkBoxShareMountPoint->setEnabled( false ) ;
	m_ui->labelMountPoint->setEnabled( false ) ;
	m_ui->lineEdit->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbMountFolder->setEnabled( false ) ;
	m_ui->pbOptions->setEnabled( false ) ;
}

void mountPartition::pbCancel()
{
	m_cancel() ;
	this->HideUI() ;
}

bool mountPartition::errorNotFound( int r )
{
	DialogMsg msg( this ) ;

	switch ( r ){
		case 0 : break ;
		case 112 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not resolve path to device or device could not be opened in read write mode" ) ) ; break ;
		case 100 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insuffienct privileges to mount the volume with given mount options" ) ) ;				break ;
		case 102 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Device already mounted" ) ) ; 						break ;
		case 103 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Insuffienct privilege to manage a system volume.\nnecessary privileges can be acquired by:\n\
1. Adding an entry for the volume in fstab with \"user\" mount option\n\2. Add yourself to \"zulumount\" group" ) ) ;					break ;
		case 104 : msg.ShowUIOK( tr( "ERROR!" ),tr( "\"/etc/fstab\" entry for this volume requires it to be mounted read only" ) ) ;		break ;
		case 113 : msg.ShowUIOK( tr( "ERROR!" ),tr( "\"/etc/fstab\" entry for this volume is malformed" ) ) ;					break ;
		case 105 : msg.ShowUIOK( tr( "ERROR!" ),tr( "\"/etc/fstab\" entry for this volume does not allow you to mount it" ) ) ;			break ;
		case 106 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Could not create mount point path,path already taken" ) ) ;				break ;
		case 114 : msg.ShowUIOK( tr( "ERROR!" ),tr( "Shared mount point path aleady taken" ) ) ;						break ;
		case 108: msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered" ) ) ;	break ;
		case 109: msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to mount ntfs/exfat file system using ntfs-3g,is ntfs-3g/exfat package installed?" ) )	      ;	break ;
		case 110: msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount failed,no or unrecognized file system" ) ) ;						break ;
		case 111: msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount failed,could not get a lock on /etc/mtab~" ) ) ;						break ;
		case 115: msg.ShowUIOK( tr( "ERROR!" ),tr( "Failed to mount the partition" ) ) ;						break ;
		default: return true ;
	}

	return false ;
}

void mountPartition::pbMount()
{
	auto test_mount = m_ui->lineEdit->text() ;

	if( test_mount.contains( "/" ) ){

		if( this->isVisible() ){

			DialogMsg msg( this ) ;
			msg.ShowUIOK( tr( "ERROR" ),tr( "\"/\" character is not allowed in the mount name field" ) ) ;
			m_ui->lineEdit->setFocus() ;
		}else{
			this->deleteLater() ;
		}
		return ;
	}

	this->disableAll() ;

	QString exe = zuluMountPath ;

	auto volume = m_path ;
	volume.replace( "\"","\"\"\"" ) ;

	if( m_ui->checkBoxShareMountPoint->isChecked() ){
		exe += " -M -m -d \"" + volume + "\"" ;
	}else{
		exe += " -m -d \"" + volume + "\"" ;
	}

	QString m = m_ui->lineEdit->text().replace( "\"","\"\"\"" ) ;

	exe += " -z \"" + m + "\"";

	if( !m_deviceOffSet.isEmpty() ){

		QString addr = utility::keyPath() ;
		utility::keySend( addr,m_key ) ;

		exe += " -o " + m_deviceOffSet + " -f " + addr ;
	}

	if( m_ui->checkBoxMountReadOnly->isChecked() ){

		exe += " -e -ro" ;
	}else{
		exe += " -e -rw" ;
	}

	if( !m_options.isEmpty() ){

		exe += " -Y " + m_options ;
	}

	auto s = utility::Task::run( utility::appendUserUID( exe ) ).await() ;

	if( s.success() ){

		m_success( utility::mountPath( m_ui->lineEdit->text() ) ) ;

		this->HideUI() ;
	}else{
		if( s.exitCode() == 103 ){

			if( utility::enablePolkit() ){

				s = utility::Task::run( utility::appendUserUID( exe ) ).await() ;
			}
		}

		if( s.success() ){

			m_success( utility::mountPath( m_ui->lineEdit->text() ) ) ;

			this->HideUI() ;
		}else{
			if( this->isVisible() ){

				if( this->errorNotFound( s.exitCode() ) ){

					QString z = s.stdOut() ;
					z.replace( tr( "ERROR: " ),"" ) ;

					DialogMsg( this ).ShowUIOK( tr( "ERROR" ),z ) ;
				}

				this->enableAll() ;
			}else{
				this->deleteLater() ;
			}
		}
	}
}

void mountPartition::showOffSetWindowOption()
{
	deviceOffset::instance( this,true,[ this ]( const QString& e,const QString& f ){

		m_deviceOffSet = e ;
		m_key = f ;
	} ) ;
}

void mountPartition::showFileSystemOptionWindow()
{
	mountOptions::instance( &m_options,this ) ;
}

void mountPartition::doAction( QAction * ac )
{
	auto e = ac->text() ;

	e.remove( "&" ) ;

	if( e == tr( "Set File System Options" ) ){
		this->showFileSystemOptionWindow() ;
	}else{
		this->showOffSetWindowOption() ;
	}
}

void mountPartition::pbOptions()
{
	m_menu->exec( QCursor::pos() ) ;
}

void mountPartition::pbOpenMountPath()
{
	auto p = tr( "Select Path To Mount Point Folder" ) ;
	auto Z = QFileDialog::getExistingDirectory( this,p,utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){

		while( true ){

			if( Z.endsWith( '/' ) ){

				Z.truncate( Z.length() - 1 ) ;
			}else{
				break ;
			}
		}

		Z = Z + "/" + m_ui->lineEdit->text().split( "/" ).last() ;
		m_ui->lineEdit->setText( Z ) ;
	}
}

void mountPartition::ShowUI( const volumeProperty& e )
{
	m_path  = e.volumeName() ;
	m_label = e.label() ;

	const auto& m = e.mountPoint() ;

	if( m.isEmpty() || m == "Nil" ){

		m_point = utility::mountPathPostFix( m_path.split( "/" ).last() ) ;
	}else{
		m_point = utility::mountPathPostFix( m ) ;
	}

	m_ui->lineEdit->setText( m_point ) ;

	bool r = m_label != "Nil" ;

	m_ui->checkBox->setEnabled( r ) ;
	m_ui->checkBox->setChecked( r && ( m.isEmpty() || m == "Nil" ) ) ;

	this->show() ;
}

void mountPartition::AutoMount( const volumeProperty& e )
{
	m_path = e.volumeName() ;

	const auto& label = e.label() ;

	const auto& m = e.mountPoint() ;

	if( m.isEmpty() || m == "Nil" ){

		if( label != "Nil" ) {

			m_point = utility::mountPathPostFix( label ) ;
		}else{
			m_point = utility::mountPathPostFix( m_path.split( "/" ).last() ) ;
		}
	}else{
		m_point = m ;
	}

	m_ui->lineEdit->setText( m_point ) ;
	this->pbMount() ;
}

void mountPartition::stateChanged( int i )
{
	Q_UNUSED( i ) ;

	m_ui->checkBox->setEnabled( false ) ;

	if( m_ui->checkBox->isChecked() ){

		m_ui->lineEdit->setText( utility::mountPathPostFix( m_label ) ) ;
	}else{
		m_ui->lineEdit->setText( utility::mountPathPostFix( m_point ) ) ;
	}

	m_ui->checkBox->setEnabled( true ) ;
}

void mountPartition::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void mountPartition::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

mountPartition::~mountPartition()
{
	delete m_ui ;
}
