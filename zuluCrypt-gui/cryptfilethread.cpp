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

#include "cryptfilethread.h"

#define TERM_ST 12
/*
 * this source file dublicated code in ../zuluCrypt-cli/bin/file_encryption.c
 * documentation for what this header file does are there.
 */

cryptfilethread::cryptfilethread(QString source,QString dest,QString keySource,QString key,QString task)
{
	m_source = source ;
	m_dest = dest ;
	m_keySource = keySource ;
	m_key = key ;
	m_task = task ;
	m_status = 0 ;
}

void cryptfilethread::start()
{
	QThreadPool::globalInstance()->start(this);
}

void cryptfilethread::terminate()
{
	m_status = TERM_ST ;
}

int cryptfilethread::encrypt()
{
	const int SIZE = 512 ;
	char buffer[SIZE];
	qint64 size ;
	struct stat st ;

	stat( m_source.toAscii().data(),&st ) ;

	size = st.st_size ;

	while( size % SIZE != 0 )
		size++ ;

	size += SIZE ;

	memset( buffer,0,SIZE ) ;

	int fd_1 = open( m_dest.toAscii().data(),O_WRONLY | O_CREAT ) ;

	int i = 0 ;
	int j = -1 ;

	emit titleUpdate(tr("creating encrypted container file"));

	for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

		if(m_status == TERM_ST){
			close( fd_1 ) ;
			remove(m_dest.toAscii().data()) ;
			return m_status ;

		}
		i = (int)(size_1 * 100 / size) ;
		if(i > j )
			emit progressUpdate(i);
		j = i ;

		write( fd_1,buffer,SIZE );
	}

	emit progressUpdate(100);
	
	close( fd_1 ) ;

	int x = this->openMapper(m_dest) ;
	
	if( x != 0){
		remove(m_dest.toAscii().data()) ;
		return x ;
	}

	int fd_2 = open(m_mapperPath.toAscii().data(),O_WRONLY);

	QString s = QString::number(st.st_size) ;

	write(fd_2,s.toAscii().data(),s.size());
	write(fd_2,'\0',1);

	fd_1 = open( "/dev/urandom",O_RDONLY ) ;
	read( fd_1,buffer,100 ) ;
	close( fd_1 ) ;

	lseek( fd_2,100,SEEK_SET ) ;

	write( fd_2,buffer,100 ) ;
	write( fd_2,buffer,100 ) ;

	lseek( fd_2,SIZE,SEEK_SET ) ;

	fd_1 = open(m_source.toAscii().data(),O_RDONLY);

	j = -1 ;
	
	emit titleUpdate(tr("copying data to the container file"));

	for( qint64 size_1 = 0 ; size_1 < size ; size_1 += SIZE ){

		if(m_status == TERM_ST){
			close( fd_1 ) ;
			close(fd_2) ;
			this->closeMapper(m_dest) ;
			remove(m_dest.toAscii().data()) ;
			return m_status ;

		}

		i = (int)(size_1 * 100 / size) ;
		if(i > j )
			emit progressUpdate(i);
		j = i ;
		read(fd_1,buffer,SIZE) ;
		write(fd_2,buffer,SIZE);

	}
	
	emit progressUpdate( 100 );
	close( fd_1 ) ;
	close( fd_2 ) ;

	this->closeMapper(m_dest) ;

	char * path = m_dest.toAscii().data() ;
	chmod( path,S_IRUSR | S_IWUSR ) ;
	chown( path,getuid(),getgid() ) ;

	m_status = 0 ;	
	return m_status ;
}

int cryptfilethread::decrypt()
{
	const int SIZE = 512 ;
	int st = this->openMapper(m_source) ;
	if( st != 0)
		return st ;

	const char * path = m_mapperPath.toAscii().data() ;

	char buffer[SIZE] ;

	int fd_1 = open(path,O_RDONLY) ;
	read( fd_1,buffer,SIZE) ;

	if(memcmp(buffer+100,buffer+200,100) != 0){
		close(fd_1);
		this->closeMapper(m_source) ;
		m_status = 11 ;
		return 11 ;

	}

	qint64 size = atoll(buffer);
	qint64 len ;
	qint64 i = 0;
	int j = 0;
	int k = -1 ;

	int fd_2 = open(m_dest.toAscii().data(),O_WRONLY  | O_CREAT);

	emit titleUpdate(tr("copying data from the container file"));

	if( size <= SIZE ){
		read( fd_1,buffer,size ) ;
		write( fd_2,buffer,size ) ;
	}else{
		len = size / SIZE ;

		for( i = 0 ; i < len ; i++ ){

			if(m_status == TERM_ST){
				close(fd_1) ;
				close(fd_2) ;
				this->closeMapper(m_source) ;
				remove(m_dest.toAscii().data()) ;
				return m_status ;

			}

			j = (int)( i * 100 / len ) ;

			if( j > k )
				emit progressUpdate(j);
			k = j ;

			read( fd_1,buffer,SIZE ) ;
			write( fd_2,buffer,SIZE ) ;
		}

		len = size - ( i * SIZE ) ;

		read( fd_1,buffer,len ) ;
		write( fd_2,buffer,len ) ;
	}

	emit progressUpdate(100);

	close(fd_1);
	close(fd_2) ;

	this->closeMapper(m_source) ;
	path = m_dest.toAscii().data() ;
	chmod( path,S_IRUSR | S_IWUSR ) ;
	chown( path,getuid(),getgid() ) ;

	m_status = 1 ;
	return m_status ;

}

int cryptfilethread::openMapper(QString path)
{
	char * p = realpath(path.toAscii().data(),NULL) ;
	if( p == NULL ){
		m_status = 7 ;
		return 7 ;
	}

	QString rpath = QString(p);
	free(p) ;

	QString e = QString("%1 -J %3 %4 -d %5").arg(ZULUCRYPTzuluCrypt).arg(m_keySource).arg(m_key).arg(path);
	QProcess exe ;
	exe.start(e);
	exe.waitForFinished();
	int st = exe.exitStatus() ;
	exe.close();
	if( st != 0 ){
		m_status = st ;
		return st ;
	}

	m_mapperPath = miscfunctions::cryptMapperPath() + QString("zuluCrypt-") + QString::number(getuid()) ;

	m_mapperPath += QString("-NAAN-") + rpath.split("/").last() + miscfunctions::hashPath(rpath);

	return 0 ;
}

int cryptfilethread::closeMapper(QString path)
{
	QString e = QString("%1 -q -d %2").arg(ZULUCRYPTzuluCrypt).arg(path);
	QProcess exe ;
	exe.start(e);
	exe.waitForFinished();
	int st = exe.exitStatus() ;
	exe.close();
	return st ;
}

void cryptfilethread::run()
{
	if(m_task == QString("-E"))
		this->encrypt();
	else
		this->decrypt();
}

cryptfilethread::~cryptfilethread()
{
	emit done(m_status) ;
}
