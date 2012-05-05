#include "erasedevicethread.h"

erasedevicethread::erasedevicethread(QString path)
{
	m_path = path ;
	m_status = 0 ;
}

void erasedevicethread::start()
{
	QThreadPool::globalInstance()->start(this);
}

void erasedevicethread::run()
{
	if( this->openMapper() != 0 )
		m_status = 2 ;
	else{
		this->writeJunkThroughMapper();
		this->closeMapper();
	}
}
#include <iostream>
void erasedevicethread::writeJunkThroughMapper()
{
	QString path = miscfunctions::cryptMapperPath() + QString("zuluCrypt-") + QString::number(getuid()) ;

	path += QString("-NAAN-") + m_path.split("/").last() + miscfunctions::hashPath(m_path);

	char * cpath = path.toAscii().data() ;

	int fd = open(cpath,O_WRONLY) ;

	if( fd < 0 )
		return ;

	const int SIZE = 1024 ;

	double size_written = 0 ;

	char buffer[SIZE];

	memset( buffer,0,SIZE ) ;

	int i ;
	int j = -1 ;

	double dev_size = ( double ) blkid_get_dev_size( fd )  ;

	while( write(fd,buffer,SIZE) > 0 ){

		if( m_status == 1 )
			break ;

		size_written += SIZE ;

		i = (int) ( size_written * 100 / dev_size ) ;

		if( i > j ){
			emit progress(i);
			j = i ;
		}

	}
	
	close(fd);
}

void erasedevicethread::closeMapper()
{
	QString exe = QString("%1 -q -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_path) ;

	QProcess p ;
	p.start(exe);
	p.waitForFinished();
	p.close();
}

int erasedevicethread::openMapper()
{
	QString exe = QString("%1 -J -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_path);

	QProcess p ;
	p.start(exe);
	p.waitForFinished();
	int st = p.exitStatus() ;
	p.close();

	return st ;
}

void erasedevicethread::cancel()
{
	m_status = 1 ;
}

erasedevicethread::~erasedevicethread()
{
	emit exitStatus(m_status);
}
