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

#include <QProcess>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QByteArray>
#include "startupupdateopenedvolumes.h"
#include "zulucrypt.h"

startupupdateopenedvolumes::startupupdateopenedvolumes()
{
}

void startupupdateopenedvolumes::start()
{
	QThreadPool::globalInstance()->start( this );
}

void startupupdateopenedvolumes::run()
{
	QProcess p ;
	p.start( QString( ZULUCRYPTzuluCrypt ) + QString( " -L" ) );
	p.waitForFinished();

	QStringList l = QString( p.readAll() ).split( "\n" ) ;

	int j = l.size() ;

	if( j == 1 )
		return ;

	QStringList entry ;
	for( int i = 0 ; i < j - 1 ; i++ ){
		entry = l.at( i ).split( "\t" );
		emit addItemToTable( entry.at( 0 ),entry.at( 1 ) ) ;
	}
}

startupupdateopenedvolumes::~startupupdateopenedvolumes()
{
	emit finished();
}
