/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
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

#ifndef ZULUCRYPTTHREADS_H
#define ZULUCRYPTTHREADS_H

#include "../zuluCrypt-cli/constants.h"
#include "ui_openpartition.h"
#include "miscfunctions.h"

#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QString>
#include <QThreadPool>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

class createFileThread : public QObject,public QRunnable
{
	Q_OBJECT
public :
	createFileThread(QString destination,double size) ;
	void start(void);
	~createFileThread();
signals:
	void exitStatus(int);
	void doneCreatingFile(void);
	void progress(int);
private slots:
	void cancelOperation(void);
private:
	void writeVolume(void);
	void openVolume(void);
	void closeVolume(void);
	void createFile(void) ;
	void fillCreatedFileWithRandomData(void);
	void run() ;

	QString m_file ;
	double m_size ;
	int m_cancelled ;
};

#endif // ZULUCRYPTTHREADS_H
