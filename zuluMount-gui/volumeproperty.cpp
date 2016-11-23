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

#include "volumeproperty.h"

#if 0
static void setFsSize( const QString& path,QString * volumeSize,QString * usedPercentage )
{
	struct statfs vfs ;

	auto passed = Task::await< bool >( [ & ](){

		return statfs( path.toLatin1().constData(),&vfs ) == 0 ;
	} ) ;

	if( passed ){

		quint64 s = vfs.f_bsize * ( vfs.f_blocks - vfs.f_bavail ) ;

		*volumeSize = utility::prettyfySpaceUsage( s ) ;

		if( vfs.f_bfree == 0 ){

			*usedPercentage = "100%" ;
		}else{
			quint64 s = vfs.f_blocks - vfs.f_bavail ;

			auto e = double( s ) / double( vfs.f_blocks ) * 100 ;

			if( e < 0.01 ){

				*usedPercentage = "0%" ;
			}else{
				*usedPercentage = QString::number( e,'f',2 ) + "%" ;
			}
		}
	}else{
		*usedPercentage = "Nil" ;
	}
}

#else

static void setFsSize( const QString& path,QString * volumeSize,QString * usedPercentage )
{
	Q_UNUSED( path ) ;

	Q_UNUSED( volumeSize ) ;

	Q_UNUSED( usedPercentage ) ;
}

#endif

bool volumeProperty::encryptedFolder(const QString& e, bool f)
{
	if( f ){

		return utility::equalsAtleastOne( e,"Nil","cryptfs","cryfs","encfs",
						  "gocryptfs","securefs","ecryptfs" ) ;
	}else{
		return utility::equalsAtleastOne( e,"cryptfs","cryfs","encfs",
						  "gocryptfs","securefs","ecryptfs" ) ;
	}
}

volumeProperty::volumeProperty( const QStringList& l,bool isSystem )
{
	this->setValues( l,isSystem ) ;
}

volumeProperty::volumeProperty( const std::initializer_list<QString>& l,bool isSystem )
{
	this->setValues( l,isSystem ) ;
}

const QString& volumeProperty::volumeName() const
{
	return m_volume ;
}

const QString& volumeProperty::mountPoint() const
{
	return m_mountPoint ;
}

const QString& volumeProperty::fileSystem() const
{
	return m_fileSystem ;
}

const QString& volumeProperty::label() const
{
	return m_label ;
}

const QString& volumeProperty::volumeSize() const
{
	return m_volumeSize ;
}

const QString& volumeProperty::spaceUsedPercentage() const
{
	return m_usedSpacePercentage ;
}

bool volumeProperty::isSystem() const
{
	return m_isSystem ;
}

bool volumeProperty::isEmpty() const
{
	return m_volume.isEmpty() ;
}

bool volumeProperty::isNotEmpty() const
{
	return !m_volume.isEmpty() ;
}

bool volumeProperty::isValid() const
{
	if( m_volume.isEmpty() ){

		return false ;
	}
	if( volumeProperty::encryptedFolder( m_fileSystem,true ) ){

		return true ;
	}
	if( utility::equalsAtleastOne( m_volumeSize,"1.0 KB","1,0 KB","0 B","Nil" ) ){

		return false ;
	}
	if( m_mountPoint == "/run/media/public/" ){

		return false ;
	}
	return true ;
}

bool volumeProperty::encryptedVolume() const
{
	return  m_fileSystem.startsWith( "crypto" ) ||
			volumeProperty::encryptedFolder( m_fileSystem,true ) ;

}

bool volumeProperty::mounted() const
{
	return m_mountPoint != "Nil" ;
}

QStringList volumeProperty::entryList() const
{
	return { m_volume,m_mountPoint,m_fileSystem,m_label,
				m_volumeSize,m_usedSpacePercentage } ;
}

volumeProperty& volumeProperty::setMountPoint( const QString& m )
{
	m_mountPoint = m ;
	return *this ;
}

void volumeProperty::setValues( const QStringList& l,bool isSystem )
{
	m_isSystem = isSystem ;

	if( l.size() >= 6 ){

		m_volume      = l.at( 0 ) ;
		m_mountPoint  = l.at( 1 ) ;
		m_fileSystem  = l.at( 2 ) ;
		m_label       = l.at( 3 ) ;
		m_volumeSize  = l.at( 4 ) ;
		m_usedSpacePercentage = l.at( 5 ) ;

		int index = m_fileSystem.indexOf( "/" ) ;

		if( index != -1 ){

			m_fileSystem = m_fileSystem.replace( "/","\n(" ) + ")" ;
		}

		m_usedSpacePercentage.remove( '\n' ) ;

		if( m_fileSystem == "cryfs" ){

			setFsSize( m_mountPoint,&m_volumeSize,&m_usedSpacePercentage ) ;
		}
	}
}
