/*
 *
 *  Copyright ( c ) 2012
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

#include "openmountpointinfilemanager.h"
#include <QMetaType>

openmountpointinfilemanager::openmountpointinfilemanager( QString folderOpener,QString path )
{
	m_path = path ;
	m_startError = 0 ;
	m_folderOpener = folderOpener ;
}

void openmountpointinfilemanager::start()
{
	QThreadPool::globalInstance()->start( this );
}

void openmountpointinfilemanager::startError( QProcess::ProcessError error )
{
	Q_UNUSED( error ) ;
	m_startError = 1 ;
}

void openmountpointinfilemanager::run()
{
	QDir dir( m_path ) ;
	if( dir.exists() ){
		QProcess * exe = new QProcess();
		m_path.replace( "\"","\"\"\"" ) ;
		//qRegisterMetaType<QProcess::ProcessError>( "QProcess::ProcessError" );
		//connect( exe,SIGNAL( error( QProcess::ProcessError ) ),this,SLOT( startError( QProcess::ProcessError ) ) ) ;
		exe->start( QString( "%1 \"%2\"" ).arg( m_folderOpener ).arg( m_path ) );
		exe->waitForFinished() ;
		m_exitCode = exe->exitCode() ;
		m_exitStatus = exe->exitStatus() ;
		exe->close();
		exe->deleteLater();
	}
}

openmountpointinfilemanager::~openmountpointinfilemanager()
{
	emit errorStatus( m_exitCode,m_exitStatus,m_startError );
}
