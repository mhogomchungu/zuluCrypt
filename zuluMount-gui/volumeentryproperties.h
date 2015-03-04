/*
 *
 *  Copyright (c) 2014
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

class volumeEntryProperties
{
public:
	explicit volumeEntryProperties( const QStringList& l = QStringList() )
	{
		this->setValues( l ) ;
	}
	const QString& volumeName() const
	{
		return m_volume ;
	}
	const QString& mountPoint() const
	{
		return m_mountPoint ;
	}
	const QString& fileSystem() const
	{
		return m_fileSystem ;
	}
	const QString& label() const
	{
		return m_label ;
	}
	const QString& volumeSize() const
	{
		return m_volumeSize ;
	}
	const QString& spaceUsedPercentage() const
	{
		return m_usedSpacePercentage ;
	}
	bool isSystem() const
	{
		return m_isSystem ;
	}
	void setisSystem( bool t = true )
	{
		m_isSystem = t ;
	}
	bool Empty() const
	{
		return m_volume.isEmpty() ;
	}
	bool notEmpty() const
	{
		return !m_volume.isEmpty() ;
	}
	bool entryisValid() const
	{
		if( m_volume.isEmpty() ){
			return false ;
		}
		if( m_volume.startsWith( "encfs" ) ){
			return true ;
		}
		if( m_volumeSize == "1.0 KB" || m_volumeSize == "Nil" ){
			return false ;
		}
		if( m_mountPoint == "/run/media/public/" ){
			return false ;
		}
		return true ;
	}
	bool encryptedVolume() const
	{
		return m_fileSystem.startsWith( "crypto" ) || m_fileSystem == "Nil" || m_fileSystem == "encfs" ;
	}
	bool mounted() const
	{
		return m_mountPoint != "Nil" ;
	}
	QStringList entryList() const
	{
		QStringList l ;
		l.append( m_volume ) ;
		l.append( m_mountPoint ) ;
		l.append( m_fileSystem ) ;
		l.append( m_label ) ;
		l.append( m_volumeSize ) ;
		l.append( m_usedSpacePercentage ) ;
		return l ;
	}
private:
	void setValues( const QStringList& l )
	{
		if( l.size() >= 6 ){
			m_volume      = l.at( 0 ) ;
			m_mountPoint  = l.at( 1 ) ;
			m_fileSystem  = l.at( 2 ) ;
			m_label       = l.at( 3 ) ;
			m_volumeSize   = l.at( 4 ) ;
			m_usedSpacePercentage = l.at( 5 ) ;

			int index = m_fileSystem.indexOf( "/" ) ;
			if( index != -1 ){
				m_fileSystem = m_fileSystem.replace( "/","\n(" ) + ")" ;
			}

			m_usedSpacePercentage.remove( QChar( '\n' ) ) ;
		}
	}
	QString m_volume ;
	QString m_mountPoint ;
	QString m_fileSystem ;
	QString m_label ;
	QString m_volumeSize ;
	QString m_usedSpacePercentage ;
	bool    m_isSystem = true ;
};

#endif // VOLUMEENTRYPROPERTIES_H
