#include "checkvolumetype.h"

checkvolumetype::checkvolumetype(QString path)
{
	m_device = path ;
}

void checkvolumetype::run()
{
	QString path = m_device ;
	path.replace("\"","\"\"\"") ;
	if( miscfunctions::isLuks(path))
		m_type = tr("luks") ;
	else
		m_type = tr("plain") ;
}

checkvolumetype::~checkvolumetype()
{
	emit done(m_type);
}
