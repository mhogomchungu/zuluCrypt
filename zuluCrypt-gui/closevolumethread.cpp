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

#include "closevolumethread.h"

#include <QProcess>
#include <unistd.h>
#include <QThreadPool>

closeVolumeThread::closeVolumeThread( QString exe )
{
	m_exe = exe ;
}

void closeVolumeThread::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void closeVolumeThread::run()
{
	QProcess p ;
	p.start( m_exe ) ;
	p.waitForFinished() ;
	m_status = p.exitCode() ;
	p.close() ;
	sleep( 1 ) ; // for ui effect
}

closeVolumeThread::~closeVolumeThread()
{
	emit finished( m_status ) ;
}
