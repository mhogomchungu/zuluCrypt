/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QProcess>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QByteArray>
#include "startupupdateopenedvolumes.h"
#include "zulucrypt.h"

startupupdateopenedvolumes::startupupdateopenedvolumes(QObject *parent) :
    QThread(parent)
{
}

QString startupupdateopenedvolumes::readMtab(QByteArray * mtab,QString entry)
{
	int i = mtab->indexOf(entry) ;
	if( i == -1 )
		return QString("") ;
	while(mtab->at(i++) != ' ') { ; }
	int j = i ;
	while(mtab->at(i++) != ' ') { ; }
	
	QByteArray m_point = mtab->mid(j, i - j - 1) ;
	m_point.replace("\\040"," ") ;
	m_point.replace("\\011","\t") ;
	m_point.replace("\\012","\n") ;
	m_point.replace("\\134","\\") ;
	return QString(m_point) ;
}

bool startupupdateopenedvolumes::checkUUID(QString *uuid,QString entry)
{
	/*
	  zuluCrypt-2468d6a7-9a71-4312-8bd9-662f982fade5
	  "entry" variable will have something like above and the code below
	  will check to see if the latter part of the string is a UUID
	  string by checking positions of the "-".
	  */

	if( entry.at(9) == QChar('-') && \
	    entry.at(18) == QChar('-') && \
	    entry.at(23) == QChar('-') && \
	    entry.at(28) == QChar('-') && \
	    entry.at(33) == QChar('-')){

		*uuid = QString("UUID=\"") + entry.mid(10)  + QString("\"");
		return true ;
	}
	return false ;
}

void startupupdateopenedvolumes::run()
{
	QStringList Z =  QDir(QString("/dev/mapper")).entryList().filter("zuluCrypt-") ;
	QProcess p ;
	QString device ;
	QString dv = QString(ZULUCRYPTzuluCrypt) + QString(" device ") ;
	QString mp ;
	QString uuid ;

	QFile mt(QString("/etc/mtab")) ;
	mt.open(QIODevice::ReadOnly) ;
	QByteArray mtab = mt.readAll() ;
	mt.close();

	for ( int i = 0 ; i < Z.size() ; i++){

		if(checkUUID(&device,Z.at(i)) == false){
			p.start( dv + Z.at(i) ) ;
			p.waitForFinished() ;
			if( p.exitCode() == 1 ){
				QString s = tr("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-") ;
				s = s + Z.at(i) ;
				s = s + tr(" because it does not look like a cryptsetup volume") ;

				emit UIMessage(tr("WARNING"), s ) ;
				continue ;
			}
			device = QString(p.readAllStandardOutput()).remove('\n')  ;
			p.close();
		}
		mp = readMtab(&mtab,Z.at(i)) ;

		if( mp == QString("") ){
			emit UIMessage(tr("WARNING"),
					  tr("An inconsitency is detected. Skipping \"") + \
					  device + \
					  tr("\" because its opened but not mounted"));
			continue ;
		}
		emit addItemToTable(device,mp) ;
	}
}

