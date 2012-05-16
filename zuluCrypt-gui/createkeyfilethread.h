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

#ifndef CREATEKEYFILETHREAD_H
#define CREATEKEYFILETHREAD_H

#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <QProcess>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

class createkeyfilethread : public QThread,public QRunnable
{
	Q_OBJECT
public:
	explicit createkeyfilethread(QString,QString);
	~createkeyfilethread();
	void start(void);
signals:
	void exitStatus(int);
public slots:
	void cancelOperation(void);
private:
	void run();
	QString m_path ;
	QString m_rng ;
	int m_cancelled ;
	int m_in ;
	int m_out ;
};

#endif // CREATEKEYFILETHREAD_H
