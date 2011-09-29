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

	void ddFinished(int, QProcess::ExitStatus) ;

private:
	Ui::createfile *ui;
	QProcess dd ;
	QTimer time ;
	double fileSize ;
};

#endif // CREATEFILE_H
