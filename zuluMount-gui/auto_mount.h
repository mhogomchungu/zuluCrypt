#ifndef AUTO_MOUNT_H
#define AUTO_MOUNT_H

#include <QString>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QStringList>
#include "bin_path.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
http://linux.die.net/man/7/inotify
http://darkeside.blogspot.com/2007/12/linux-inotify-example.html
 */

class auto_mount : public QThread
{
	Q_OBJECT
public:
	explicit auto_mount( QObject * parent = 0 );
	~auto_mount() ;
	static bool deviceIsSystem( QString ) ;
signals:
	void deviceAdded( QString ) ;
	void deviceRemoved( QString ) ;
	void getVolumeSystemInfo( QStringList ) ;
	void getVolumeInfo( QStringList ) ;
private:
	bool deviceMatchLVMFormat( QString device ) ;
	void volumeProperties( QString ) ;
	void deviceFromDev( uint32_t,QString ) ;
	void deviceFromDevMapper( uint32_t,QString ) ;
	const char * m_device ;
	void run() ;
	int m_fdDir ;
	char * m_buffer ;
};

#endif // AUTO_MOUNT_H
