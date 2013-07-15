/*
 *
 *  Copyright ( c ) 2012
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "openvolumereadonly.h"
#include <QDebug>
//openvolumereadonly::openvolumereadonly( QObject * parent )
//{
//	this->setParent( parent );
//}

int openvolumereadonly::setOption( QWidget * parent, int state,QString app )
{
	QString Path = QDir::homePath() + QString( "/.zuluCrypt/" ) + app ;
	QFile f( Path + QString( "-openMode" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( QString::number( state ).toAscii() ) ;
	f.close();

	DialogMsg msg( parent ) ;
	QString m = QObject::tr( "setting this option will cause the volume to open in read only mode" ) ;

	QString path = Path + QString( "-readOnlyOption" ) ;

	f.setFileName( path ) ;

	bool st ;
	if( f.exists() ){
		f.open( QIODevice::ReadWrite ) ;
		QByteArray opt = f.readAll() ;
		if( opt == QByteArray( "0" ) && state == 2 ) {
			f.seek( 0 ) ;

			st = msg.ShowUIOKDoNotShowOption( QObject::tr( "info" ),m ) ;

			st ? f.write( "1" ) : f.write( "0" ) ;

			f.close();
		}
	}else{
		st = msg.ShowUIOKDoNotShowOption( QObject::tr( "info" ),m ) ;

		f.open( QIODevice::WriteOnly ) ;

		st ? f.write( "1" ) : f.write( "0" ) ;

		f.close();
	}

	return state ;
}

Qt::CheckState openvolumereadonly::getOption( QString app )
{
	QString home = QDir::homePath() + QString( "/.zuluCrypt/" ) ;
	QDir d( home ) ;
	if( !d.exists() ){
		d.mkdir( home ) ;
	}
	QFile f( home + app + QString( "-openMode" ) ) ;

	if( f.exists() == false ){
		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write( QString::number( Qt::Unchecked ).toAscii() ) ;
		f.close();
	}

	f.open( QIODevice::ReadOnly ) ;
	int st = QString( f.readAll() ).toInt() ;

	f.close();
	return ( Qt::CheckState ) st ;
}
