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

#include "createfilethread.h"
#include "miscfunctions.h"

#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <iostream>
#include <QDir>
#include <cstdio>
#include "zulucrypt.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

createFileThread::createFileThread(QString source,QString file,double size,int type)
{
	m_source = source ;
	m_file = file ;
	m_size = size ;
	m_type = type ;
	m_in = open( m_source.toAscii().data(),O_RDONLY) ;
	m_out = open( m_file.toAscii().data(),O_WRONLY | O_CREAT) ;

	chmod(m_file.toAscii().data(),S_IRWXU);
}

void createFileThread::createKeyFile()
{
	char data ;
	for( int i = 0 ; i < m_size ; i++){
		do{
			read(m_in,&data,1);
		}while( data < 32 || data > 126) ;

		write(m_out,&data,1);
	}
}

void createFileThread::createContainer()
{
	double count = m_size / 1024 ;
	char data[1024];

	for(double i = 0 ; i < count ; i++){
		read(m_in,data,1024);
		write(m_out,data,1024);
	}
}

void createFileThread::run()
{
	if( m_type == 0 )
		createKeyFile();
	else
		createContainer();
}

createFileThread::~createFileThread()
{
	close(m_in);
	close(m_out);
}
