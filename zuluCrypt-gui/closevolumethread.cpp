#include "closevolumethread.h"

closeVolumeThread::closeVolumeThread(QString exe)
{
	m_exe = exe ;
	m_status = -1 ;
}

void closeVolumeThread::run()
{
	QProcess p ;
	p.start(m_exe);
	p.waitForFinished() ;
	m_status = p.exitCode() ;
	p.close();
	sleep(1); //volume closes too fast, put a second pause to show ui effect on closing
}

closeVolumeThread::~closeVolumeThread()
{
	emit finished(m_status);
}
