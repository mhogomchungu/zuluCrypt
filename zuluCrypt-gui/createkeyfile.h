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


#ifndef CREATEKEYFILE_H
#define CREATEKEYFILE_H

#include <QWidget>

#include "zulucryptthreads.h"

namespace Ui {
    class createkeyfile;
}

class createkeyfile : public QWidget
{
	Q_OBJECT

public:
	explicit createkeyfile(QWidget *parent = 0);
	~createkeyfile();

public slots:

	void ShowUI(void) ;
	void HideUI(void) ;

private slots:

	void pbCancel(void) ;
	void pbCreate(void) ;
	void pbOpenFolder(void) ;
	void threadfinished(void);
	void threadterminated(void);

private:

	void disableAll(void);

	void enableAll(void) ;

	Ui::createkeyfile *ui;

	rngThread *rng ;

	QString in ;
	QString out ;
	QString path ;
};

#endif // CREATEKEYFILE_H
