#include "volumepropertiesthread.h"

volumePropertiesThread::volumePropertiesThread(QString path,QString mpoint,QObject *parent) :
        QThread(parent)
{
	m_path = path.replace("\"","\"\"\"") ; ;
	m_mpoint = mpoint ;
}

void volumePropertiesThread::run()
{
	QString z = QString(ZULUCRYPTzuluCrypt) ;
	z = z + QString(" status ") ;
	z = z + QString("\"") ;
	z = z + m_path + QString("\"");

	QProcess p ;
	p.start( z ) ;
	p.waitForFinished() ;

	QByteArray t("\n") ;
	QByteArray r = p.readAllStandardOutput() ;

	int start = r.length() - r.indexOf(t) - 2 ;

	p.close();

	p.start("df -PTh ");

	p.waitForFinished() ;

	QStringList df = QString(p.readAllStandardOutput()).split("\n").filter(m_mpoint) ;

	p.close();

	QByteArray c ;

	c = df.at(0).toAscii() ;
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

	QString y = QString(" fs:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n size:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n used:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n available:\t") + QString(c.mid(j,i - j - 1)) ;

	while ( c.at(i++) == ' ') { ; }
	j = i - 1 ;
	while ( c.at(i++) != ' ') { ; }

	y = y + QString("\n used%:\t") + QString(c.mid(j,i - j - 1));

	QStringList l = miscfunctions::luksEmptySlots(m_path) ;

	QString properties ;
	if ( miscfunctions::isLuks(m_path) == true){
		QString x =  QString(" ") ;
		x = x + QString( r.right(start) ) ;
		x = x + QString(" used slots:\t") ;
		x = x + l.at(0) + QString(" / ") + l.at(1) + QString("\n");

		properties = x + y;
	}else
		properties = QString(" ") + QString( r.right(start) ) + y;

	emit finished(properties,this);
}
