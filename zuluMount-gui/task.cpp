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

#include "task.h"
#include <QDebug>

#include <QProcess>
#include <QThreadPool>
#include <QDir>

#include "../zuluCrypt-gui/lxqt_wallet/frontend/lxqt_wallet.h"
#include "../zuluCrypt-gui/utility.h"
#include "bin_path.h"
#include <unistd.h>

Task::Task()
{
}

void Task::setMode( const QString& mode )
{
	m_mode = mode ;
}

void Task::setDevice( const QString& path )
{
	m_device = path ;
}

void Task::setType( const QString& type )
{
	m_type = type ;
}

void Task::setKeySource( const QString& key )
{
	m_keySource = key ;
}

void Task::setMountPoint( const QString& m )
{
	m_point = m ;
	m_point.replace( "\"","\"\"\"" ) ;
}

void Task::setMakeMountPointPublic( bool opt )
{
	m_publicMount = opt ;
}

void Task::setList( const QStringList& l )
{
	m_list = l ;
}

void Task::setMountPointOpener( const QString& opener )
{
	m_folderOpener = opener ;
}

void Task::setWallet( lxqt::Wallet::Wallet * wallet )
{
	m_wallet = wallet ;
}

void Task::setVolumeID( const QString& id )
{
	m_volumeID = id ;
}

void Task::openPathInFileManager()
{
	QProcess p ;
	QString exe = QString( "xdg-open ") + m_point ;
	p.start( exe ) ;
	p.waitForFinished() ;
	p.close() ;
}

void Task::run()
{
	switch( m_action ){
		case Task::Update              : return this->partitionList() ;
		case Task::Mount               : return this->mount() ;
		case Task::Unmount             : return this->umount() ;
		case Task::CryptoOpen          : return this->cryptoOpen() ;
		case Task::VolumeProperties    : return this->volumeProperties() ;
		case Task::VolumeMiniProperties: return this->volumeMiniProperties() ;
		case Task::OpenPath            : return this->openPathInFileManager() ;
		case Task::CheckPermissions    : return this->checkPermissions() ;
		case Task::VolumeType          : return this->getVolumeType() ;
		case Task::systemdevice        : return this->checkIfSystemDevice() ;
		case Task::checkUnMount        : return this->checkUnmount() ;
		case Task::openMountPoint      : return this->openMountPointTask() ;
		case Task::getKey              : return this->getKeyTask() ;
	}
}

void Task::getKeyTask()
{
	m_key.clear() ;

	if( m_volumeID.startsWith( QString( "UUID=" ) ) ){
		m_key = m_wallet->readValue( m_volumeID ) ;
		if( m_key.isEmpty() ){
			m_key = m_wallet->readValue( m_volumeID.replace( "\"","" ) ) ;
		}
	}else{
		QString uuid = utility::getUUIDFromPath( m_volumeID ) ;
		if( uuid.isEmpty() ){
			m_key = m_wallet->readValue( m_volumeID ) ;
		}else{
			m_key = m_wallet->readValue( uuid ) ;
			if( m_key.isEmpty() ){
				m_key = m_wallet->readValue( m_volumeID ) ;
			}
		}
	}
}

void Task::openMountPointTask()
{
	QProcess exe ;
	exe.start( QString( "%1 \"%2\"" ).arg( m_folderOpener ).arg( m_point ) ) ;
	exe.waitForFinished() ;
	m_exitCode = exe.exitCode() ;
	m_exitStatus = exe.exitStatus() ;
	exe.close() ;
}

void Task::checkUnmount()
{
	QProcess p ;
	QString exe = QString( "%1 -c -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;
	p.start( exe ) ;
	p.waitForFinished() ;
}

void Task::checkIfSystemDevice( void )
{
	QProcess p ;
	QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
	p.start( exe ) ;
	p.waitForFinished() ;
	QString s = QString( p.readAll() ) ;
	p.close() ;
	QStringList l = s.split( "\n" ) ;
	int j = l.size() ;
	m_systemDevice = false ;
	for( int i = 0 ; i < j ; i++ ){
		if( l.at( i ) == m_device ){
			m_systemDevice = true ;
			break ;
		}
	}
}

void Task::getVolumeType()
{
	this->checkIfSystemDevice() ;

	QProcess p ;
	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;
	p.start( exe ) ;
	p.waitForFinished() ;

	QString m = p.readAll() ;
	QStringList l ;
	if( p.exitCode() == 0 ){
		l =  m.split( "\t" ) ;
		if( l.size() >= 4 ){
			if( m_systemDevice ){
				emit getVolumeSystemInfo( l ) ;
			}else{
				emit getVolumeInfo( l ) ;
			}
		}
	}else{
		emit getVolumeSystemInfo( l ) ;
		emit getVolumeInfo( l ) ;
	}
}

void Task::checkPermissions()
{
	QProcess p ;
	p.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -C" ) ) ;
	p.waitForFinished() ;
	emit checkPermissions( p.exitCode() ) ;
	p.close() ;
}

void Task::partitionList()
{
	QProcess p ;
	QProcess q ;

	QStringList k ;
	QStringList j ;

	p.start( QString( "%1 -l" ).arg( zuluMount ) ) ;

	if( p.waitForFinished( 10000 ) ){
		k = QString( p.readAll() ).split( '\n' ) ;
		q.start( QString( "%1 -S" ).arg( zuluMount ) ) ;
		if( q.waitForFinished( 10000 ) ){
			j = QString( q.readAll() ).split( '\n' ) ;
		}
		q.close() ;
	}

	p.close() ;

	emit signalMountedList( k,j ) ;
}

void Task::volumeProperties()
{
	QProcess p ;
	QString exe ;

	exe = QString( "%1 -s -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString r ;

	if( p.exitCode() == 0 ){
		r = p.readAll() ;
	}else{
		;
	}

	emit signalProperties( r ) ;
}

void Task::volumeMiniProperties()
{
	QProcess p ;
	QString exe ;
	//sleep( 1 ) ; for UI effect
	exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString r ;
	if( p.exitCode() == 0 ){
		r = p.readAll() ;
	}else{
		;
	}

	emit signalProperties( r ) ;
}

void Task::cryptoOpen()
{
	QProcess p ;
	QString exe ;

	QString d = m_device.replace( "\"","\"\"\"" ) ;

	exe = QString( "%1 -m -d \"%2\" -z \"%3\" -e %4 %5" ).arg( zuluMount ).arg( d ).arg( m_point ).arg( m_mode ).arg( m_keySource ) ;

	if( m_publicMount ){
		exe = exe + QString( " -M" ) ;
	}

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 ){
		output = output.mid( index + 1 ) ;
	}
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close() ;
}

void Task::mount()
{
	QProcess p ;
	QString exe ;

	QString d = m_device.replace( "\"","\"\"\"" ) ;

	if( m_point.isEmpty() ){
		m_point = QDir::homePath() + QString( "/" ) + d.split( "/" ).last() ;
	}

	exe = QString( "%1 -m -d \"%2\" -e %3 -z \"%4\"" ).arg( zuluMount ).arg( m_device ).arg( m_mode ).arg( m_point ) ;

	if( m_publicMount ){
		exe = exe + QString( " -M" ) ;
	}

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 ){
		output = output.mid( index + 1 ) ;
	}
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close() ;
}

void Task::umount()
{
	QProcess p ;
	QString exe = QString( "%1 -u -d \"%2\"" ).arg( zuluMount ).arg( m_device.replace( "\"","\"\"\"" ) ) ;

	p.start( exe ) ;
	p.waitForFinished( -1 ) ;

	QString output_1 = QString( p.readAll() ) ;
	int index = output_1.indexOf( QChar( ':' ) ) ;
	output_1 = output_1.mid( index + 1 ) ;
	p.close() ;

	emit signalUnmountComplete( p.exitCode(),output_1 ) ;
}

void Task::start( Task::Action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

Task::~Task()
{
	emit done() ;
	emit errorStatus( m_exitCode,m_exitStatus,m_startError ) ;
	emit key( m_key ) ;
}
