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

#include "additemtotablethread.h"
#include <QMutexLocker>

#include "zulucrypt.h"


addItemToTableThread::addItemToTableThread(QMutex *mt,QTableWidget *tw, QString d, QString m, int * i, int * s)
{
	device = d;
	m_point = m;
	item_count = i ;
	selectedRow = s ;
	tableWidget = tw ;
	mutex = mt ;
}

void addItemToTableThread::run()
{
	mutex->lock();

	int tc = *item_count ;
	int sr = *selectedRow ;

	tableWidget->insertRow(tc);
	tableWidget->setItem(tc,0,new QTableWidgetItem(device)) ;
	tableWidget->setItem(tc,1,new QTableWidgetItem(m_point)) ;

	if ( zuluCrypt::isLuks( device.replace("\"","\"\"\"") ) == true )
		tableWidget->setItem(tc,2,new QTableWidgetItem(tr("luks"))) ;
	else
		tableWidget->setItem(tc,2,new QTableWidgetItem(tr("plain"))) ;

	tableWidget->item(tc,0)->setTextAlignment(Qt::AlignCenter);
	tableWidget->item(tc,1)->setTextAlignment(Qt::AlignCenter);
	tableWidget->item(tc,2)->setTextAlignment(Qt::AlignCenter);

	if( tc == 0 ){
		tableWidget->item(0,0)->setSelected(true);
		tableWidget->item(0,1)->setSelected(true);
		tableWidget->item(0,2)->setSelected(true);
		*selectedRow = 0 ;
	}else{
		tableWidget->item(tc,0)->setSelected(true);
		tableWidget->item(tc,1)->setSelected(true);
		tableWidget->item(tc,2)->setSelected(true);
		tableWidget->item(sr,0)->setSelected(false);
		tableWidget->item(sr,1)->setSelected(false);
		tableWidget->item(sr,2)->setSelected(false);
		*selectedRow = *item_count ;
	}

	*item_count = *item_count + 1 ;

	mutex->unlock();

	emit threadFinished(this) ;
}
