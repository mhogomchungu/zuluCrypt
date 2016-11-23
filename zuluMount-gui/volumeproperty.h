/*
 *
 *  Copyright (c) 2014-2015
 *  name : Francis Banyikwa
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
#ifndef VOLUMEENTRYPROPERTIES_H
#define VOLUMEENTRYPROPERTIES_H

#include <QString>
#include <QStringList>

#include <initializer_list>

#include <sys/vfs.h>

#include "../zuluCrypt-gui/utility.h"

class volumeProperty
{
public:
	static bool encryptedFolder( const QString& e,bool f ) ;

	volumeProperty( const QStringList& l = QStringList(),bool isSystem = false ) ;
	volumeProperty( const std::initializer_list<QString>& l,bool isSystem = false ) ;

	const QString& volumeName() const ;
	const QString& mountPoint() const ;
	const QString& fileSystem() const ;
	const QString& label() const ;
	const QString& volumeSize() const ;
	const QString& spaceUsedPercentage() const ;

	bool isSystem() const ;
	bool isEmpty() const ;
	bool isNotEmpty() const ;
	bool isValid() const ;
	bool encryptedVolume() const ;
	bool mounted() const ;
	QStringList entryList() const ;
	volumeProperty& setMountPoint( const QString& m ) ;
private:
	void setValues( const QStringList& l,bool isSystem ) ;

	QString m_volume ;
	QString m_mountPoint ;
	QString m_fileSystem ;
	QString m_label ;
	QString m_volumeSize ;
	QString m_usedSpacePercentage ;
	bool    m_isSystem = true ;
};

#endif // VOLUMEENTRYPROPERTIES_H
