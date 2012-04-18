#include "erasedevicethread.h"

erasedevicethread::erasedevicethread(QString path)
{
	m_path = path ;
	m_status = 0 ;
	m_prev_ratio = -1 ;
	m_timer = new QTimer(this);
	connect(m_timer,SIGNAL(timeout()),this,SLOT(progressTimer()));
	m_timer->start(250);
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

void erasedevicethread::writeJunkThroughMapper()
{
	m_size = ( double ) blkid_get_dev_size( m_id );

	m_size_written = 0 ;

	const size_t SIZE = 1024 ;
	size_t size ;

	char buffer[SIZE];
	memset( buffer,0,SIZE ) ;

	while( 1 ){

		if( m_size - m_size_written <= SIZE ){
			for( size = m_size - m_size_written ; size != 0 ; )
				size = write( m_id,buffer,size ) ;
			break ;
		}

		m_size_written += SIZE ;

		for( size = SIZE ; size != 0 ; )
			size = write( m_id,buffer,size ) ;
	}
	close(m_id);
}

void erasedevicethread::closeMapper()
{
	QString exe = QString("%1 -q -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_path) ;
	runInThread * rt = new runInThread(exe);
	QThreadPool::globalInstance()->start(rt);
}

int erasedevicethread::openMapper()
{
	QString exe = QString("%1 -J -d \"%2\"").arg(ZULUCRYPTzuluCrypt).arg(m_path);

	QProcess p ;
	p.start(exe);
	p.waitForFinished();
	int st = p.exitStatus() ;
	p.close();

	if( st != 0 )
		return 1 ;
	
	QString path = miscfunctions::cryptMapperPath() + QString("zuluCrypt-") + QString::number(getuid()) ;

	path += QString("-NAAN-") + m_path.split("/").last() + miscfunctions::hashPath(m_path);

	char * cpath = path.toAscii().data() ;

	m_id = open(cpath,O_WRONLY) ;

	return 0 ;
}

void erasedevicethread::progressTimer()
{
	m_ratio = ( int ) ( ( m_size_written / m_size ) * 100 ) ;

	if( m_ratio != m_prev_ratio )
		emit progress(m_ratio);

	m_prev_ratio = m_ratio ;
}

void erasedevicethread::cancel()
{
	this->terminate();
	close(m_id);
	this->closeMapper();
	m_status = 1 ;
}

erasedevicethread::~erasedevicethread()
{
	m_timer->stop();
	m_timer->deleteLater();
	miscfunctions::debug("~erasedevicethread()");
	emit exitStatus(m_status);
}
