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

startupupdateopenedvolumes::startupupdateopenedvolumes()
{
}

bool startupupdateopenedvolumes::checkUUID(QString *uuid,QString entry)
{
	if(entry.mid(0,15) == QString("zuluCrypt-UUID-")){
		*uuid = QString("UUID=\"") + entry.mid(15)  + QString("\"");
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
	QString ddv = QString(ZULUCRYPTzuluCrypt) + QString(" checkUUID ") ;
	QString mp ;
	QString uuid ;

	QFile mt(QString("/etc/mtab")) ;
	mt.open(QIODevice::ReadOnly) ;
	QByteArray mtab = mt.readAll() ;
	mt.close();
	QString entry ;
	int status ;
	for ( int i = 0 ; i < Z.size() ; i++){
		entry = Z.at(i);
		if(checkUUID(&device,entry) == false){
			p.start( dv + entry ) ;
			p.waitForFinished() ;
			status = p.exitCode() ;
			device = QString(p.readAllStandardOutput()).remove('\n')  ;
			p.close();
			if( status == 1 ){
				QString s = tr("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-%1 \
					       because it does not look like a cryptsetup volume").arg(entry) ;
				UIMessage(tr("WARNING"), s ) ;
				continue ;
			}
		}else{
			p.start(ddv + device);
			p.waitForFinished() ;
			status = p.exitCode() ;
			p.close();
			if( status == 1 ){
				QString s = tr("An inconsitency is detected, skipping /dev/mapper/zuluCrypt-%1 \
					       because its UUID does not match any UUID from attached partitions").arg(entry) ;
				UIMessage(tr("WARNING"), s ) ;
				continue ;
			}
		}
		mp = miscfunctions::readMtab(&mtab,entry) ;
		if( mp == QString("") ){
			UIMessage(tr("WARNING"),
					  tr("An inconsitency is detected. Skipping \"") + \
					  device + \
					  tr("\" because its opened but not mounted"));
			continue ;
		}
		emit addItemToTable(device,mp) ;
	}
	emit finished();
}

