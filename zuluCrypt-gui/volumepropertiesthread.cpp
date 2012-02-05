#include "volumepropertiesthread.h"

volumePropertiesThread::volumePropertiesThread(QString path,QString mpoint)
{
	m_path = path.replace("\"","\"\"\"") ; ;
	m_mpoint = mpoint ;
}

void volumePropertiesThread::run()
{
	QString z = QString(ZULUCRYPTzuluCrypt) + QString(" status ") + \
			QString("\"") + m_path + QString("\"");

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;

	QByteArray data = p.readAllStandardOutput() ;

	p.close();

	QString volumeProperties = QString(" ") + QString(data.mid(data.indexOf('\n') + 2));

	p.start("df -PTh ");
	p.waitForFinished() ;

	QStringList df = QString(p.readAllStandardOutput()).split("\n").filter(m_mpoint) ;

	p.close();

	QByteArray c = df.at(0).toAscii() ;
	/*
	  Below code will attemp to walk through a line like below without
	  caring how many space characters are involved in separating items:

	  /dev/mapper/zuluCrypt-image-02.img ext4   99G   96G  1.9G  99% /home/ink/image-02.img

	*/
	int i = 0 ;
	int j ;
	while ( c.at(i++) != ' ') { ; }
	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	volumeProperties += QString(" fs:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	volumeProperties += QString("\n size:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	volumeProperties += QString("\n used:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	volumeProperties += QString("\n available:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	volumeProperties += QString("\n used%:\t") + QString(c.mid(j,i - j - 1));

	emit finished(volumeProperties);
}
