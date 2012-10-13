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

#ifndef VOLUMEPROPERTIESTHREAD_H
#define VOLUMEPROPERTIESTHREAD_H

#include <QThreadPool>
#include <QRunnable>
#include <QString>
#include <QStringList>
#include <sys/types.h>
#include <unistd.h>
#include <QProcess>
#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/bash_special_chars.h"

class volumePropertiesThread : public QObject,public QRunnable
{
	Q_OBJECT
public:
	volumePropertiesThread( QString,QString );
	void start( void );
	~volumePropertiesThread();
signals:
	void finished( QString ) ;
public slots:	
private:
	void run( void );
	QString m_fusefs ;
	QString m_path ;
	QString m_mpoint ;
	QString m_volumeProperties ;
};

#endif // VOLUMEPROPERTIESTHREAD_H
