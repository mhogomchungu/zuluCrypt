/*
 *
 *  Copyright ( c ) 2012
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

#ifndef ERASEDEVICETHREAD_H
#define ERASEDEVICETHREAD_H

#include <QObject>
#include <QRunnable>
#include <QString>
#include <QProcess>
#include <QThreadPool>
#include <QFile>

#include "miscfunctions.h"
#include "runinthread.h"
#include "../zuluCrypt-cli/constants.h"

#include <blkid/blkid.h>
#include <string.h>

class erasedevicethread : public QObject,public QRunnable
{
	Q_OBJECT
public:
	explicit erasedevicethread( QString );
	void start( void );
	~erasedevicethread();
signals:
	void progress( int );
	void exitStatus( int ) ;
public slots:
	void cancel( void );
private:
	int openMapper( void );
	void writeJunkThroughMapper( void );
	void closeMapper( void );
	void run( void );
	int m_status ;
	QString m_path ;
};

#endif // ERASEDEVICETHREAD_H
