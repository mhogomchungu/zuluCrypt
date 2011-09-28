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


#ifndef OPEN_PARTITION_H
#define OPEN_PARTITION_H


#include "ui_openpartition.h"


class openpartition :  public QDialog
{
	Q_OBJECT

public:

	openpartition(QWidget *parent = 0);

	virtual ~openpartition();

signals :

	void clickedPartition(QString) ;

public slots:

	void tableEntryDoubleClicked(int row, int column) ;

	void ShowUI(void);

	void ShowNonSystemPartitions(QStringList) ;

private:

	void HideUI(void);

	Ui::PartitionView partitionView ;

	int count ;
};

#endif
