/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>

#include "zulucryptthreads.h"

namespace Ui {
    class createfile;
}

class createfile : public QWidget
{
	Q_OBJECT

public:
	explicit createfile(QWidget *parent = 0);
	~createfile();

signals :

	void fileCreated(QString) ;

public slots:

	void showUI(void) ;

	void pbOpenFolder(void) ;

	void pbCancel(void) ;

	void pbCreate(void) ;

private slots:

	void monitorFileGrowth(void);

	void createFileThreadFinished(void) ;

private:
	void enableAll(void) ;
	void disableAll(void) ;
	Ui::createfile *ui;
	QTimer time ;
	double fileSize ;
	void closeEvent(QCloseEvent *) ;
	bool creating ;
	bool terminated ;
	bool Return ;
	QString path ;
	QMessageBox mb ;
	createFileThread *cft ;
};

#endif // CREATEFILE_H
