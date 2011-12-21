/*
 *
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
#include "../zuluCrypt-cli/executables.h"

#include <iostream>
#include <QProcess>
#include <QStringList>

closeAllVolumesThread::closeAllVolumesThread(QTableWidget *t)
{
	table = t ;
}
void closeAllVolumesThread::run()
{		
	table->setEnabled(false);

	int i = table->rowCount() ;

	int j = -1 ;

	QTableWidgetItem * tableItems[ i ] ;

	QTableWidgetItem * deviceItem ;

	while( ++j < i )
		tableItems[ j ] = table->item(j,0) ;

	QProcess p ;

	QString exe ;

	QString device ;

	for( j = 0 ; j < i ; j++ ){

		deviceItem = tableItems[ j ] ;

		device = deviceItem->text().replace("\"","\"\"\"") ;

		exe = QString(ZULUCRYPTzuluCrypt) + QString(" close ") + QString("\"") + \
			       device  + QString("\"") ;

		p.start( exe );

		p.waitForFinished() ;

		emit close(deviceItem,p.exitCode()) ;

		p.close();

		//sleep(1) ;
	}
	table->setEnabled(true);
}
