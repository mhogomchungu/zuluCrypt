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

#include "additemtotablethread.h"
#include <QMutexLocker>

#include "zulucrypt.h"
#include "miscfunctions.h"

addItemToTableThread::addItemToTableThread(QMutex *mt,QTableWidget *tw, QString d, QString m)
{
	device = d;
	m_point = m;
	tableWidget = tw ;
	mutex = mt ;
}

void addItemToTableThread::run()
{
	mutex->lock();
	int row = tableWidget->rowCount() ;
	tableWidget->insertRow(row);
	tableWidget->setItem(row,0,new QTableWidgetItem(device)) ;
	tableWidget->setItem(row,1,new QTableWidgetItem(m_point)) ;

	if ( miscfunctions::isLuks( device.replace("\"","\"\"\"") ) == true )
		tableWidget->setItem(row,2,new QTableWidgetItem(tr("luks"))) ;
	else
		tableWidget->setItem(row,2,new QTableWidgetItem(tr("plain"))) ;

	tableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
	tableWidget->item(row,1)->setTextAlignment(Qt::AlignCenter);
	tableWidget->item(row,2)->setTextAlignment(Qt::AlignCenter);

	tableWidget->setCurrentCell(row,1);
	mutex->unlock();
	emit threadFinished(this) ;
}
