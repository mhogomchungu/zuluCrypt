/*
 *
 *  Copyright (c) 2012
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

#ifndef CHECKVOLUMETYPE_H
#define CHECKVOLUMETYPE_H

#include <QRunnable>
#include <QObject>
#include <QString>
#include <QThreadPool>

#include "utility.h"

class checkvolumetype : public QObject,public QRunnable
{
	Q_OBJECT
public:
	checkvolumetype( QString );
	~checkvolumetype() ;
	void start( void );
signals:
	void complete( QString );
private:
	QString m_device ;
	QString m_type ;
	void run( void );
};

#endif // CHECKVOLUMETYPE_H
