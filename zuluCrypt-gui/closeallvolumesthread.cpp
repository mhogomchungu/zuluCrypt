/*
 *
 *  Copyright (c) 2011
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

#include "closeallvolumesthread.h"
#include "../zuluCrypt-cli/constants.h"

closeAllVolumesThread::closeAllVolumesThread( QTableWidget * table )
{
	m_table = table ;
}

void closeAllVolumesThread::start()
{
	QThreadPool::globalInstance()->start( this );
}

void closeAllVolumesThread::run()
{
	m_table->setEnabled( false );
	sleep( 1 ) ; // for ui effect
	int i = m_table->rowCount() ;

	if( i < 1 ){
		m_table->setEnabled( true );
		return ;
	}

	int j = -1 ;

	QVector<QTableWidgetItem*> tableItems( 0 ) ;

	QTableWidgetItem * deviceItem ;

	while( ++j < i ){
		tableItems.append( m_table->item( j,0 ) );
	}
	
	QProcess p ;
	QString exe ;
	QString device ;

	for( j = 0 ; j < i ; j++ ){
		deviceItem = tableItems.at( j ) ;
		device = deviceItem->text().replace( "\"","\"\"\"" ) ;
		exe = QString( "%1 -q -d \"%2\"" ).arg( ZULUCRYPTzuluCrypt ).arg( device ) ;
		p.start( exe );
		p.waitForFinished() ;
		emit close( deviceItem,p.exitCode() ) ;
		p.close();
		sleep( 1 ) ; // for ui effect
	}
	
	m_table->setEnabled( true );
}
