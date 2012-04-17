#include "createkeyfilethread.h"
#include "miscfunctions.h"

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
		do{
			read(m_in,&data,1);
		}while( data < 32 || data > 126) ;

		while( write(m_out,&data,1) != 1 ) { ; }
	}
}

void createkeyfilethread::cancelOperation()
{
	m_cancelled = 1 ;
	this->terminate();
}

createkeyfilethread::~createkeyfilethread()
{
	close(m_in);
	close(m_out);
	emit exitStatus(m_cancelled);
}
