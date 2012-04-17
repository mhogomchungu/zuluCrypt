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
#include <unistd.h>

createFileThread::createFileThread(QString file,double size)
{
	m_cancelled = 0 ;
	m_file = file ;
	m_size = size / 1024 ;
}

void createFileThread::getKey()
{
	int x = open("/dev/urandom",O_RDONLY) ;
	char k ;
	for( int j = 0 ; j < 64 ; j++ ){
		do{
			read(x,&k,1);
		}while(k < 32 || k > 126) ;
		m_key[j] = k ;
	}

	close(x);
	m_key[64] = '\0' ;
}

void createFileThread::cancelOperation()
{
	this->terminate();

	m_cancelled = 1 ;

	close(m_pid);
	this->closeVolume() ;
}

void createFileThread::run()
{
	/*
	 * This is the entry point for this class.
	 * The class basically cr*eates a file of a size specified by a user by writing '\0' to it.
	 *
	 * Then it creates a cryptsetup mapper and writes '\0' to the file through the mapper.
	 *
	 * This medhod of writing random data to the file seem to be faster than creating the file using random
	 * data from /dev/urandom 
	 */
	memset(m_data,0,1024);

	this->createFile();

	emit doneCreatingFile();

	this->getKey();

	this->fillCreatedFileWithRandomData();
}

void createFileThread::createFile()
{
	size_t size ;
	double i ;
	int x = open( m_file.toAscii().data(),O_WRONLY | O_CREAT ) ;

	for(i = 0 ; i < m_size ; i++){
		for( size = 1024 ; size != 0 ; )
			size = size - write(x,m_data,size);
	}

	close(x);
	chmod(m_file.toAscii().data(),S_IRWXU);
}

void createFileThread::fillCreatedFileWithRandomData()
{
	this->openVolume()  ;
	
	this->writeVolume() ;
	
	this->closeVolume() ;
}

void createFileThread::closeVolume()
{
	/*
	 * not using qprocess like when opening the volume to remove UI hanging while the volume is being closed
	 */
	QString exe = QString("%1 -q -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_file) ;
	runInThread * rt = new runInThread(exe);
	QThreadPool::globalInstance()->start(rt);
}

void createFileThread::openVolume()
{
	QString exe = QString("%1 -O -d \"%2\" -e rw -p %3").arg(ZULUCRYPTzuluCrypt).arg(m_file).arg(QString(m_key));

	QProcess p ;
	p.start(exe);
	p.waitForFinished();
	p.close();
}

void createFileThread::writeVolume()
{
	QString path = miscfunctions::cryptMapperPath() + QString("zuluCrypt-") + QString::number(getuid()) ;
	
	path += QString("-NAAN-") + m_file.split("/").last() + miscfunctions::hashPath(m_file);
	
	m_pid = open(path.toAscii().data(),O_WRONLY) ;
	
	int Z ;
	
	size_t size ;
	double i ;
	for(i = 0 ; i < m_size ; i++){
		
		Z = (int)( i / m_size  * 100 ) ;
		
		if( Z % 5 == 0 )
			emit progress(Z);

		for( size = 1024 ; size != 0 ; )
			size = size - write(m_pid,m_data,size);
	}
	
	close(m_pid);
}

createFileThread::~createFileThread()
{	
	emit exitStatus(m_cancelled);
}
