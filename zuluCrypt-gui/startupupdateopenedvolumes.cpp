/*
 *
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
#include "startupupdateopenedvolumes.h"
#include "zulucrypt.h"

startupupdateopenedvolumes::startupupdateopenedvolumes(QObject *parent) :
    QThread(parent)
{
}

void startupupdateopenedvolumes::run()
{
	QStringList Z =  QDir(QString("/dev/mapper")).entryList().filter("zuluCrypt-") ;

	QProcess p ;

	QString device ;

	QString dv = QString(ZULUCRYPTzuluCrypt) + QString(" device ") ;

	QString mp ;

	for ( int i = 0 ; i < Z.size() ; i++){

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

		mp = zuluCrypt::mtab(Z.at(i)) ;

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

