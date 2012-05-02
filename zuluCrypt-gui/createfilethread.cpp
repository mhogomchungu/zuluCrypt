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
	m_size = size ;
}

void createFileThread::cancelOperation()
{
	m_cancelled = 1 ;
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

	this->createFile();

	emit doneCreatingFile();

	this->fillCreatedFileWithRandomData();
}

void createFileThread::createFile()
{
	size_t size ;
	double i ;

	int fd = open( m_file.toAscii().data(),O_WRONLY | O_CREAT ) ;

	memset(m_data,0,1024);

	double k = m_size / 1024 ;
	for(i = 0 ; i < k ; i++){
		if(m_cancelled == 1)
			break ;
		for( size = 1024 ; size != 0 ; )
			size = size - write(fd,m_data,size);
	}

	close(fd) ;
	chmod(m_file.toAscii().data(),S_IRWXU);
}

void createFileThread::timerSignal()
{
	emit progress(( int ) ( m_data_written * 100 / m_size ));
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
	 * not using qprocess here because the operations will hang for a moment and the hanging
	 * will show up on the UI
	 */
	QString exe = QString("%1 -q -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_file) ;
	runInThread * rt = new runInThread(exe);
	QThreadPool::globalInstance()->start(rt);
}

void createFileThread::openVolume()
{
	/*
	 * We do not let the cli write random data to the file by using -X(we use -J)because we want to write the random data
	 * ourselves giving us the ability to knoe exactly much data is already written
	 */
	QString exe = QString("%1 -J -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_file);

	QProcess p ;
	p.start(exe);
	p.waitForFinished();
	p.close();
}

void createFileThread::writeVolume()
{
	if(m_cancelled == 1)
		return ;

	QString path = miscfunctions::cryptMapperPath() + QString("zuluCrypt-") + QString::number(getuid()) ;
	
	path += QString("-NAAN-") + m_file.split("/").last() + miscfunctions::hashPath(m_file);
	
	int fd = open(path.toAscii().data(),O_WRONLY) ;
	
	int j ;
	int k = -1 ;
	m_data_written = 0 ;

	while(write(fd,m_data,1024) > 0){

		m_data_written += 1024 ;

		j = ( int )( m_data_written * 100 / m_size ) ;

		if( j > k ){
			emit progress( j );
			k = j ;
		}

		if(m_cancelled == 1)
			break ;
	}
	close(fd);
}

createFileThread::~createFileThread()
{
	emit exitStatus(m_cancelled);
}
