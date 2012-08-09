#include "keystrength.h"

keystrength::keystrength()
{
#if BUILD_PWQUALITY
	m_handle = pwquality_default_settings() ;
#endif
}

bool keystrength::canCheckQuality()
{
#if BUILD_PWQUALITY
	return true ;
#else
	return false ;
#endif
}

int keystrength::quality( QString key )
{
#if BUILD_PWQUALITY
	void * auxerror ;
	const char * p = key.toAscii().constData() ;
	int st = pwquality_check( m_handle,p,NULL,NULL,&auxerror);
	pwquality_strerror( NULL,0,st,auxerror);
	return st ;
#else
	return NOT_USED ;
#endif
}

keystrength::~keystrength()
{
#if BUILD_PWQUALITY
	pwquality_free_settings( m_handle );
#endif
}
