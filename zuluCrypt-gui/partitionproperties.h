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

#ifndef PARTITIONPROPERTIES_H
#define PARTITIONPROPERTIES_H

#include <QRunnable>
#include <QTableWidget>
#include <QProcess>
#include <QString>
#include <QStringList>
#include "utility.h"
#include <QThreadPool>

#include <unistd.h>

class partitionproperties : public QObject,public QRunnable
{
	Q_OBJECT
public:
	explicit partitionproperties( QString );
	void start( void );
	~partitionproperties();
signals:
	void finished( void );
	void partitionProperties( QStringList );
public slots:
private:
	void run( void );
	QString m_partitionType ;
};

#endif // PARTITIONPROPERTIES_H
