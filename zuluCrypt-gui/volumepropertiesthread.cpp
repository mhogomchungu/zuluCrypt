#include "volumepropertiesthread.h"

volumePropertiesThread::volumePropertiesThread(QString path,QString mpoint)
{
	m_fusefs = path ;
	m_path = path.replace("\"","\"\"\"") ;
	m_mpoint = mpoint ;
}

QString volumePropertiesThread::fuseblkGetFs()
{
	QString fuse = QString("/dev/mapper/zuluCrypt-") ;
	if( m_fusefs.mid(0,5) == QString("UUID=") ){
		m_fusefs.remove(QChar('"'));
		fuse += QString("UUID-") + m_fusefs.mid(5) ;
	}else{
		//replace bash special characters with '_' to workaround cryptsetup bug
		QString p = QString(BASH_SPECIAL_CHARS);
		QString f = m_fusefs.split("/").last() ;
		int g = p.size() ;
		for( int i = 0 ; i < g ; i++ ){
			f.replace(p.at(i),QChar('_'));
		}
		fuse += f ;
	}
	QStringList stl = miscfunctions::deviceProperties(fuse) ;
	return stl.at(3) ;
}

void volumePropertiesThread::run()
{
	QString z = QString(ZULUCRYPTzuluCrypt) + QString(" -s -d ") + QString("\"") + m_path + QString("\"");

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;

	QByteArray data = p.readAllStandardOutput() ;

	p.close();

	m_volumeProperties = QString(" ") + QString(data.mid(data.indexOf('\n') + 2));

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

	QString fs = QString(c.mid(j,i - j - 1)) ;
	if( fs == QString("fuseblk") ){
		/*
		  ntfs-3g gives a file system of volumes in mounts as "fuseblk" in /etc/mtab.
		  This means file systems like ntfs will show up as "fuseblk" and this is not ideal.
		  This function go to the mapper and read the file system using blkid to present the real fs.

		  Net result:users will see "ntfs" instead of "fuseblk" when opening ntfs volumes.
		  */
		m_volumeProperties += QString(" fs:\t") + fuseblkGetFs();
	}else
		m_volumeProperties += QString(" fs:\t") +  fs ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	m_volumeProperties += QString("\n size:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	m_volumeProperties += QString("\n used:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	m_volumeProperties += QString("\n available:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	m_volumeProperties += QString("\n used%:\t") + QString(c.mid(j,i - j - 1));
}

volumePropertiesThread::~volumePropertiesThread()
{
	emit finished(m_volumeProperties);
}
