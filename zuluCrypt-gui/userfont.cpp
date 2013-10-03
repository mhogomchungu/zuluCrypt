/*
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

#include "userfont.h"

#include <QFile>
#include <QDir>
#include <QWidget>
#include <QString>
#include <QFont>

userfont::userfont( QWidget * parent )
{
	m_parent = parent ;
}

QFont userfont::getFont()
{
	QString fontPath = QDir::homePath() + QString( "/.zuluCrypt/font" ) ;
	QFile x( fontPath ) ;
	if( x.open( QIODevice::ReadOnly ) == false ){
		return m_parent->font() ;
		/*
		x.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		QString s = QString( "Sans Serif\n8\nnormal\nnormal\n" ) ;
		x.write( s.toAscii() ) ;
		x.close() ;
		*/
	}else{
		QStringList xs = QString( x.readAll() ).split( "\n" ) ;
		x.close() ;
		QFont F ;
		F.setFamily( xs.at( 0 ) ) ;
		F.setPointSize( xs.at( 1 ).toInt() ) ;
		if( xs.at( 2 ) == QString( "normal" ) ){
			F.setStyle( QFont::StyleNormal ) ;
		}else if( xs.at( 2 ) == QString( "italic" ) ){
			F.setStyle( QFont::StyleItalic ) ;
		}else{
			F.setStyle( QFont::StyleOblique ) ;
		}
		if( xs.at( 3 ) == QString( "normal" ) ){
			F.setWeight( QFont::Normal ) ;
		}else{
			F.setWeight( QFont::Bold ) ;
		}
		return F ;
	}
}

void userfont::saveFont( const QFont& Font )
{
	int k = Font.pointSize() ;
	QString s = QString( "%1\n%2\n" ).arg( Font.family() ).arg( QString::number( k ) ) ;

	if( Font.style() == QFont::StyleNormal ){
		s = s + QString( "normal\n" ) ;
	}else if( Font.style() == QFont::StyleItalic ){
		s = s + QString( "italic\n" ) ;
	}else{
		s = s + QString( "oblique\n" ) ;
	}
	if( Font.weight() == QFont::Normal ){
		s = s + QString( "normal\n" ) ;
	}else{
		s = s + QString( "bold" ) ;
	}
	QFile f( QDir::homePath() + QString( "/.zuluCrypt/font" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( s.toAscii() ) ;
	f.close() ;
}
