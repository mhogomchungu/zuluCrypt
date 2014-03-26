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

#include "getKey.h"

#include <sys/types.h>
#include <signal.h>
#include <QDebug>

getKey::getKey( const QString& exe,QByteArray * key,const QString& keyFile )
{
	m_exe = exe ;
	m_key = key ;
	m_keyFile = keyFile ;
	m_cancelled = false ;
}

void getKey::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void getKey::setKeyRoutine( std::function<QByteArray( const QString& exe,const QString& keyFile,const QString& password )> function )
{
	m_function = function ;
}

void getKey::run()
{
	*m_key = m_function( m_exe,m_keyFile,QString( *m_key ) ) ;
}

void getKey::cancel()
{
	kill( m_pid,SIGTERM ) ;
	m_cancelled = true ;
}

getKey::~getKey()
{
	emit doneReadingKey( m_cancelled ) ;
}
