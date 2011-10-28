
#include <QProcess>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <iostream>
#include "startupupdateopenedvolumes.h"
#include "zulucrypt.h"

startupupdateopenedvolumes::startupupdateopenedvolumes(QObject *parent) :
    QThread(parent)
{
}

void startupupdateopenedvolumes::run()
{
	QStringList Z =  QDir(QString("/dev/mapper")).entryList().filter("zuluCrypt-") ;

	char *c, *d, *v ;

	QProcess * p ;

	for ( int i = 0 ; i < Z.size() ; i++){

		QString C = Z.at(i) ;

		C = C.right( C.length() - 10 ) ; //10 is the length of "zuluCrypt-

		p = new QProcess() ;

		p->start(QString(ZULUCRYPTzuluCrypt) + QString(" status ") + C ) ;

		p->waitForFinished() ;

		c = p->readAllStandardOutput().data() ;

		p->close();

		p->deleteLater();

		d = strstr(c,"device:") ;

		if ( d == NULL){
			QString s = tr("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-") ;
			s = s + QString(C) ;
			s = s + tr(" because it does not look like a cryptsetup volume") ;

			emit UIMessage(tr("WARNING"), s ) ;

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

		char volume[ strlen( v ) + 1 ] ;

		d = volume ;

		while ( ( *d++ = *v++ ) != '\0') { ; }

		v = strrchr(volume,'/') + 1 ;

		int j = 0 ;

		while( *( volume + j++) != '\0') { ; }

		char bff[j] ;

		strcpy(bff,v) ;

		for( int n = 0 ; n < j ; n++){

			if( bff[n] == ' ')
				bff[n] = '_' ;
		}

		QString mp = zuluCrypt::mtab(QString(bff)) ;

		if( mp == QString("") ){
			emit UIMessage(tr("WARNING"),
					  tr("An inconsitency is detected. Skipping \"" + \
					  QString(volume) + \
					  tr("\" because its opened but not mounted")));
			continue ;
		}
		emit addItemToTable(QString(volume),mp) ;
	}
}

