
/*
 *
 *  Copyright (c) 2014-2015
 *  name : Francis Banyikwa
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

#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QObject>
#include <QFile>

auto gpg = []( const QVector<QString>& exe,const QString& keyFile,const QString& password ){

	QString arg ;

	if( password.isEmpty() ){
		arg = QString( "%1 --no-tty --yes --no-mdc-warning --no-verbose -d %2" ).arg( exe.first(),keyFile ) ;
	}else{
		arg = QString( "%1 --no-tty --yes --no-mdc-warning --no-verbose --passphrase-fd 0 -d  %2" ).arg( exe.first(),keyFile ) ;
	}

	QProcess p ;

	p.start( arg ) ;

	p.waitForStarted() ;

	p.write( password.toLatin1() ) ;
	p.closeWriteChannel() ;
	p.waitForFinished() ;
	return p.readAllStandardOutput() ;
} ;
