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

#ifndef ADDITEMTOTABLETHREAD_H
#define ADDITEMTOTABLETHREAD_H

#include <QThread>
#include <QMutex>
#include <QTableWidget>

class addItemToTableThread : public QThread
{
	Q_OBJECT
public:
	addItemToTableThread(QMutex *,QTableWidget *, QString, QString, int *, int *) ;
signals:
	void threadFinished(addItemToTableThread *) ;
private:
	void run(void) ;
	QString device ;
	QString m_point ;
	int * item_count ;
	int * selectedRow ;
	QTableWidget * tableWidget ;
	QMutex *mutex ;
};

#endif // ADDITEMTOTABLETHREAD_H
