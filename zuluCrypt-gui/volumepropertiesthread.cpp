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

#include "volumepropertiesthread.h"

#include <QThreadPool>
#include <QProcess>
#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/bash_special_chars.h"

volumePropertiesThread::volumePropertiesThread( QString path,QString mpoint )
{
	m_fusefs = path ;
	m_path = path.replace( "\"","\"\"\"" ) ;
	m_mpoint = mpoint ;
}

void volumePropertiesThread::start()
{
	QThreadPool::globalInstance()->start( this );
}

void volumePropertiesThread::run()
{
	QString z = QString( "%1 -s -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( m_path ) ;

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;
	int st = p.exitCode() ;
	QByteArray data = p.readAllStandardOutput() ;

	p.close();

	if( st != 0 ){
		m_volumeProperties = QString( "" ) ;
	}else{
		m_volumeProperties = QString( " " ) + QString( data.mid( data.indexOf( '\n' ) + 2 ) );
	}
}

volumePropertiesThread::~volumePropertiesThread()
{
	emit finished( m_volumeProperties );
}

