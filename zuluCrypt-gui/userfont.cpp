#include "userfont.h"

userfont::userfont( QWidget * parent )
{
	m_parent = parent ;
}

QFont userfont::getFont()
{
	QString fontPath = QDir::homePath() + QString( "/.zuluCrypt/font" ) ;
	QFile x( fontPath );
	if( x.open( QIODevice::ReadOnly ) == false ){
		return m_parent->font() ;
		/*
		x.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		QString s = QString( "Sans Serif\n8\nnormal\nnormal\n" ) ;
		x.write( s.toAscii() ) ;
		x.close();
		*/
	}else{
		QStringList xs = QString( x.readAll() ).split( "\n" ) ;
		x.close();
		QFont F ;
		F.setFamily( xs.at( 0 ) );
		F.setPointSize( xs.at( 1 ).toInt() );
		if( xs.at( 2 ) == QString( "normal" ) )
			F.setStyle( QFont::StyleNormal );
		else if( xs.at( 2 ) == QString( "italic" ) )
			F.setStyle( QFont::StyleItalic );
		else
			F.setStyle( QFont::StyleOblique );

		if( xs.at( 3 ) == QString( "normal" ) )
			F.setWeight( QFont::Normal );
		else
			F.setWeight( QFont::Bold );

		return F ;
	}
}

void userfont::saveFont( QFont Font )
{
	int k = Font.pointSize() ;
	QString s = QString( "%1\n%2\n" ).arg( Font.family() ).arg( QString::number( k ) ) ;

	if( Font.style() == QFont::StyleNormal )
		s = s + QString( "normal\n" ) ;
	else if( Font.style() == QFont::StyleItalic )
		s = s + QString( "italic\n" ) ;
	else
		s = s + QString( "oblique\n" ) ;

	if( Font.weight() == QFont::Normal )
		s = s + QString( "normal\n" ) ;
	else
		s = s + QString( "bold" ) ;

	QFile f( QDir::homePath() + QString( "/.zuluCrypt/font" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( s.toAscii() ) ;
	f.close();
}
