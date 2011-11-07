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
#include <iostream>

closeAllVolumesThread::closeAllVolumesThread(QTableWidget *t)
{
	table = t ;
}
void closeAllVolumesThread::run()
{
	table->setEnabled(false);

	sleep(1) ;

	int i = table->rowCount() ;

	int j ;

	/*
	  the table changed its dimensions everytime a row is deleted and
	  hence deleting items by row number is difficult.
	  The below will read all entries in the table and then attempt to
	  close each entry making table changes in row numbers not important.
	  */
	QTableWidgetItem *rowEntries[ i ] ;

	for( j = 0 ; j < i ; j++ ){

		rowEntries[j] = table->item(j,0) ;
	}

	for( j = 0 ; j < i ; j ++ ){

		emit close(rowEntries[j]) ;

		sleep(2) ;
	}

	table->setEnabled(true);
}
