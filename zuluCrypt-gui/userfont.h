#ifndef USERFONT_H
#define USERFONT_H

#include <QFont>
#include <QFile>
#include <QDir>
#include <QWidget>
#include <QString>

class userfont
{
public:
	userfont( QWidget * );
	QFont getFont() ;
	void saveFont( QFont ) ;
private :
	QWidget * m_parent ;
};

#endif // USERFONT_H
