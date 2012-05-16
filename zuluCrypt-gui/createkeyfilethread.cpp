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

#include "createkeyfilethread.h"
#include "miscfunctions.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

createkeyfilethread::createkeyfilethread(QString path,QString rng)
{
	m_path = path ;
	m_rng = rng ;
	m_cancelled = 0 ;
}

void createkeyfilethread::run()
{
	char data ;
	char * path = m_path.toAscii().data() ;

	m_in = open(m_rng.toAscii().data(),O_RDONLY) ;
	m_out = open(path,O_WRONLY|O_CREAT);

	chmod(path,S_IRWXU);

	for( int i = 0 ; i < 64 ; i++){
		if(m_cancelled == 1)
			break ;
		do{
			read(m_in,&data,1);
		}while( data < 32 || data > 126) ;

		while( write(m_out,&data,1) != 1 ) { ; }
	}
	close(m_in);
	close(m_out);
}

void createkeyfilethread::start()
{
	QThreadPool::globalInstance()->start(this);
}

void createkeyfilethread::cancelOperation()
{
	m_cancelled = 1 ;
}

createkeyfilethread::~createkeyfilethread()
{
	emit exitStatus(m_cancelled);
}
