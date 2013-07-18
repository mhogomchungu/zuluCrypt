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

#include "managepartitionthread.h"
#include <QDebug>

managepartitionthread::managepartitionthread()
{
}

void managepartitionthread::setMode( QString mode )
{
	m_mode = mode ;
}

void managepartitionthread::setDevice( QString path )
{
	m_device = path ;
}

void managepartitionthread::setType( QString type )
{
	m_type = type ;
}

void managepartitionthread::setKeySource( QString key )
{
	m_keySource = key ;
}

void managepartitionthread::setMountPoint( QString m )
{
	m_point = m ;
}

void managepartitionthread::setMakeMountPointPublic( bool opt )
{
	m_publicMount = opt ;
}

void managepartitionthread::setList( QStringList l )
{
	m_list = l ;
}

void managepartitionthread::openPathInFileManager()
{
	QProcess p ;
	QString exe = QString( "xdg-open ") + m_point ;
	p.start( exe );
	p.waitForFinished() ;
	p.close();
}

void managepartitionthread::run()
{
	switch( m_action ){
		case managepartitionthread::Update              : return this->partitionList();
		case managepartitionthread::Mount               : return this->mount();
		case managepartitionthread::Unmount             : return this->umount( m_type );
		case managepartitionthread::CryptoOpen          : return this->cryptoOpen();
		case managepartitionthread::VolumeProperties    : return this->volumeProperties();
		case managepartitionthread::VolumeMiniProperties: return this->volumeMiniProperties() ;
		case managepartitionthread::OpenPath            : return this->openPathInFileManager();
		case managepartitionthread::CheckPermissions    : return this->checkPermissions();
		case managepartitionthread::VolumeType          : return this->getVolumeType();
		case managepartitionthread::systemdevice        : return this->checkIfSystemDevice();
		case managepartitionthread::checkUnMount        : return this->checkUnmount();
	}
}

void managepartitionthread::checkUnmount()
{
	QProcess p ;
	QString exe = QString( "%1 -c -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;
	p.start( exe );
	p.waitForFinished() ;
}

void managepartitionthread::checkIfSystemDevice( void )
{
	QProcess p ;
	QString exe = QString( "%1 -S" ).arg( zuluMount ) ;
	p.start( exe );
	p.waitForFinished() ;
	QString s = QString( p.readAll() ) ;
	p.close();
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

void managepartitionthread::getVolumeType()
{
	this->checkIfSystemDevice() ;

	QProcess p ;
	QString exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;
	p.start( exe );
	p.waitForFinished() ;

	QString m = p.readAll() ;
	QStringList l ;
	if( p.exitCode() == 0 ){
		l =  m.split( "\t" ) ;
		if( l.size() >= 4 ){
			if( m_systemDevice ){
				emit getVolumeSystemInfo( l ) ;
			}else{
				emit getVolumeInfo( l );
			}
		}
	}else{
		emit getVolumeSystemInfo( l ) ;
		emit getVolumeInfo( l );
	}
}

void managepartitionthread::checkPermissions()
{
	QProcess p ;
	p.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -C" ) );
	p.waitForFinished() ;
	emit checkPermissions( p.exitCode() );
	p.close();
}

void managepartitionthread::partitionList()
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
		q.close();
	}

	p.close();

	emit signalMountedList( k,j ) ;
}

void managepartitionthread::volumeProperties()
{
	QProcess p ;
	QString exe ;

	exe = QString( "%1 -s -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;

	if( p.exitCode() ){
		QString failed ;
		emit signalProperties( failed );
	}else{
		emit signalProperties( output );
	}

	p.close();
}

QString managepartitionthread::volumeMiniProperties_1()
{
	QProcess p ;
	QString exe ;
	//sleep( 1 ) ; for UI effect
	exe = QString( "%1 -L -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished( -1 ) ;

	if( p.exitCode() == 0 ){
		return QString( p.readAll() ) ;
	}else{
		return QString( "" ) ;
	}
}

void managepartitionthread::volumeMiniProperties()
{
	signalProperties( this->volumeMiniProperties_1() ) ;
}

void managepartitionthread::cryptoOpen()
{
	QProcess p ;
	QString exe ;

	exe = QString( "%1 -m -d \"%2\" -z \"%3\" -e %4 %5" ).arg( zuluMount ).arg( m_device ).arg( m_point ).arg( m_mode ).arg( m_keySource ) ;

	if( m_publicMount ){
		exe = exe + QString( " -M" ) ;
	}

	p.start( exe );
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 ){
		output = output.mid( index + 1 ) ;
	}
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::mount()
{
	QProcess p ;
	QString exe ;

	if( m_point.isEmpty() ){
		m_point = QDir::homePath() + QString( "/" ) + m_device.split( "/" ).last() ;
	}

	exe = QString( "%1 -m -d \"%2\" -e %3 -z \"%4\"" ).arg( zuluMount ).arg( m_device ).arg( m_mode ).arg( m_point ) ;

	if( m_publicMount ){
		exe = exe + QString( " -M" ) ;
	}

	p.start( exe );
	p.waitForFinished( -1 ) ;

	QString output = QString( p.readAll() ) ;
	int index = output.indexOf( QChar( ':') ) ;
	if( index != -1 ){
		output = output.mid( index + 1 ) ;
	}
	emit signalMountComplete( p.exitCode(),output ) ;
	p.close();
}

void managepartitionthread::umount( QString type )
{
	Q_UNUSED( type ) ;
	QProcess p ;
	QString exe = QString( "%1 -u -d \"%2\"" ).arg( zuluMount ).arg( m_device ) ;

	p.start( exe );
	p.waitForFinished( -1 ) ;

	QString output_1 = QString( p.readAll() ) ;
	int index = output_1.indexOf( QChar( ':' ) ) ;
	output_1 = output_1.mid( index + 1 ) ;
	p.close();

	emit signalUnmountComplete( p.exitCode(),output_1 ) ;
}

void managepartitionthread::startAction( managepartitionthread::Action action )
{
	m_action = action ;
	QThreadPool::globalInstance()->start( this ) ;
}

managepartitionthread::~managepartitionthread()
{
	emit done();
}
