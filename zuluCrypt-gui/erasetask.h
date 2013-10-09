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

class EraseTask : public QObject,public QRunnable
{
	Q_OBJECT
public:
	explicit EraseTask( QString ) ;
	void start( void ) ;
	~EraseTask() ;
signals:
	void progress( int ) ;
	void exitStatus( int ) ;
public slots:
	void cancel( void ) ;
private:
	int writeJunk( void ) ;
	int openMapper( void ) ;
	void writeJunkThroughMapper( void ) ;
	void closeMapper( void ) ;
	void run( void ) ;
	int m_status ;
	QString m_path ;
};

#endif // ERASEDEVICETHREAD_H
