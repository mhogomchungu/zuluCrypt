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

#ifndef BACKUPLUKSHEADER_H
#define BACKUPLUKSHEADER_H

#include <QWidget>
#include <QThreadPool>
#include <QFileDialog>

#include "runinthread.h"
#include "miscfunctions.h"
#include "openpartition.h"

namespace Ui {
    class backupluksheader;
}

class backupluksheader : public QWidget
{
	Q_OBJECT
public:
	explicit backupluksheader(QWidget *parent = 0);
	~backupluksheader();
signals:
	void HideUISignal(void);
public slots:
	void ShowUI(void) ;
	void HideUI(void) ;

private slots:
	void pbCancel(void) ;
	void pbCreate(void) ;
	void pbOpenFolder(void) ;
	void threadExitStatus(int);
	void pbOpenPartition(void);
	void pbOpenFile(void);
	void selectedPartition(QString);
private:
	void closeEvent(QCloseEvent *) ;
	void disableAll(void);
	void enableAll(void) ;
	Ui::backupluksheader *m_ui;

	QString m_path ;
	/*
	  prototyped at miscfunctions.h
	  */
	UIMsg m_msg ;

	bool m_backUpInProgress ;
};

#endif // CREATEKEYFILE_H
