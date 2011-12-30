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

#ifndef STARTUPUPDATEOPENEDVOLUMES_H
#define STARTUPUPDATEOPENEDVOLUMES_H

#include <QThread>

class startupupdateopenedvolumes : public QThread
{
	Q_OBJECT
public:
	explicit startupupdateopenedvolumes(QObject *parent = 0);	
signals:
	void addItemToTable(QString,QString) ;
	void UIMessage(QString title,QString message) ;
	void finished(startupupdateopenedvolumes *);
private:
	void run() ;
	QString readMtab(QByteArray *,QString) ;
	bool checkUUID(QString *,QString ) ;
};

#endif // STARTUPUPDATEOPENEDVOLUMES_H
