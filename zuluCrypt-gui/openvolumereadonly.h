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

#ifndef OPENVOLUMEREADONLY_H
#define OPENVOLUMEREADONLY_H

#include <QObject>
#include <QString>
#include <Qt>

class QWidget ;

class openvolumereadonly : public QObject
{
public:
	//openvolumereadonly( QObject * parent );

	static int setOption( QWidget * parent, int state,QString app ) ;
	static Qt::CheckState getOption( QString app ) ;
};

#endif // OPENVOLUMEREADONLY_H
