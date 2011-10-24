
#include <QProcess>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QMessageBox>

#include "startupupdateopenedvolumes.h"
#include "zulucrypt.h"

startupupdateopenedvolumes::startupupdateopenedvolumes(QObject *parent) :
    QThread(parent)
{
}

void startupupdateopenedvolumes::run()
{
	QStringList Z =  QDir(QString("/dev/mapper")).entryList().filter("zuluCrypt-") ;

	char *c, *d, *v,*volume,*N ;

	QProcess * p ;

	for ( int i = 0 ; i < Z.size() ; i++){

		QString C = Z.at(i) ;

		C = C.right( C.length() - 10 ) ; //10 is the length of "zuluCrypt-

		p = new QProcess() ;

		p->start(QString(ZULUCRYPTzuluCrypt) + QString(" status ") + C ) ;

		p->waitForReadyRead() ;

		c = p->readAllStandardOutput().data() ;

		p->close();

		p->deleteLater();

		d = strstr(c,"device:") ;

		if ( d == NULL){
			QString s = QString("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-") ;
			s = s + QString(C) ;
			s = s + QString(" because it does not look like a cryptsetup volume") ;

			emit UIMessage(QString("WARNING"), s ) ;

			continue ;
		}

		while ( *++d != '/') { ; }

		if (strncmp(d ,"/dev/loop",9) != 0 ){
			v = d ;
		}else{
			v = strstr(c,"loop:")  ;

			while(*++v != '/') { ; }
		}

		d = v ;

		while ( *++d != '\n') { ; }

		*d = '\0' ;

		N = d = volume = new char[ strlen( v ) + 1 ] ;

		while ( ( *d++ = *v++ ) != '\0') { ; }

		p = new QProcess() ;

		p->start(QString(ZULUCRYPTmount));

		p->waitForReadyRead() ;

		c = p->readAllStandardOutput().data() ;

		v = strrchr(volume,'/') + 1 ;

		if( ( d = strstr(c,v ) ) == NULL ){
			emit UIMessage(QString("WARNING"),
				  QString("An inconsitency is detected, /dev/mapper/zuluCrypt-" + \
					  QString(N) + QString(" is opened but not mounted")));
			continue ;
		}

		v = d = d + strlen(v) + 4 ;

		while ( *++v != ' ') { ; }

		*v = '\0' ;

		emit addItemToTable(QString(volume),QString(d)) ;

		delete [] volume ;

		p->close();

		p->deleteLater();
	}
}

