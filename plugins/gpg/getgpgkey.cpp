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

#include "getgpgkey.h"

#include <sys/types.h>
#include <signal.h>

getgpgkey::getgpgkey( const QString& exe,QByteArray * key,const QString& keyFile )
{
	m_gpgExe = exe ;
	m_key = key ;
	m_keyFile = keyFile ;
	m_cancelled = false ;
}

void getgpgkey::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void getgpgkey::run()
{
	QProcess exe ;
	QString arg ;

	if( m_key->isEmpty() ){
		arg = QString( "%1 --no-tty --yes --no-mdc-warning --no-verbose -d %2" ).arg( m_gpgExe ).arg( m_keyFile ) ;
	}else{
		arg = QString( "%1 --no-tty --yes --no-mdc-warning --no-verbose --passphrase-fd 0 -d  %2" ).arg( m_gpgExe ).arg( m_keyFile ) ;
	}

	exe.start( arg ) ;

	exe.waitForStarted() ;

	m_pid = exe.pid() ;

	exe.write( *m_key ) ;
	exe.closeWriteChannel() ;
	exe.waitForFinished( -1 ) ;
	*m_key = exe.readAllStandardOutput() ;
}

void getgpgkey::cancel()
{
	kill( m_pid,SIGTERM ) ;
	m_cancelled = true ;
}

getgpgkey::~getgpgkey()
{
	emit doneReadingFromgpg( m_cancelled ) ;
}
