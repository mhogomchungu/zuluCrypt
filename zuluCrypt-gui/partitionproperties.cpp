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

#include "partitionproperties.h"

#include <QTableWidget>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThreadPool>

#include <unistd.h>

#include "utility.h"
#include "../zuluCrypt-cli/constants.h"

partitionproperties::partitionproperties( QString partitionType )
{
	m_partitionType = partitionType ;
}

void partitionproperties::start()
{
	QThreadPool::globalInstance()->start( this );
}

void partitionproperties::run()
{
	QProcess p ;

	/*
	  Root user can create encrypted volumes in all partitions including system partitions.
	  Show all partitions, not only non system.
	  */
	if( m_partitionType == QString( " -N" ) && getuid() == 0 ){
		m_partitionType = QString( " -A" ) ;
	}
	p.start( QString( ZULUCRYPTzuluCrypt ) + m_partitionType + QString( " -Z" ) ) ;

	p.waitForFinished() ;

	QStringList l = QString( p.readAllStandardOutput() ).split( "\n" ) ;

	p.close();
	int j = l.size() - 1 ;
	if( j < 1 ){
		return ;
	}
	QStringList list ;
	QString entry ;
	for ( int i = 0 ; i < j ; i++ ){
		list = l.at( i ).split( "\t" ) ;
		entry = list.at( 3 ) ;
		/*
		 * MDRAID partitions have "linux_raid_member" as their file system
		 * LVM partitions have "LVM2_member" as their file system
		 *
		 * we are not showing these partitions since we dont support them
		 */
		if( entry.contains( QString( "member" ) ) ){
			;
		}else{
			emit partitionProperties( list );
		}
	}
}

partitionproperties::~partitionproperties()
{
	emit finished();
}
