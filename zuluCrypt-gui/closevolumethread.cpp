#include "closevolumethread.h"

closeVolumeThread::closeVolumeThread(QString exe)
{
	m_exe = exe ;
}

void closeVolumeThread::run()
{
	QProcess p ;
	p.start(m_exe);
	p.waitForFinished() ;
	int status = p.exitCode() ;
	p.close();
	sleep(1); //volume closes too fast, put a second pause to show ui effect on closing
	emit finished(status);
}
