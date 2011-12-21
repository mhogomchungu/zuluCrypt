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

#ifndef ZULUCRYPTTHREADS_H
#define ZULUCRYPTTHREADS_H

#include "../zuluCrypt-cli/executables.h"
#include "ui_openpartition.h"

#include <QThread>
#include <QFile>
#include <QMetaType>
#include <QTableWidget>
#include <QTableWidgetItem>

class ClickedRowHighlight : public QThread
{
public :
	ClickedRowHighlight(void) ;
	void update(int,QTableWidget*, int *,int) ;
private:
	void run() ;
	int row ;
	QTableWidget *tableWidget ;
	int *selectedRow ;
	int item_count ;
};

class zuluCryptThreads : public QThread
{
public:
    explicit zuluCryptThreads(QObject *parent = 0);
};

class ShowNonSystemPartitionsThread : public QThread
{
public :
	void run() ;
	ShowNonSystemPartitionsThread(Ui::PartitionView *,QFont) ;
private:
	Ui::PartitionView * partitionView ;
	QFont font ;
};

class partitionlistThread : public QThread
{
public :
	void run() ;
	partitionlistThread(Ui::PartitionView *,QFont) ;
private:
	Ui::PartitionView * partitionView ;
	QFont font ;
};

class createFileThread : public QThread
{
public :
	createFileThread(QString source,QString destination,double size,int type) ;
private:
	void run() ;
	QString source ;
	QString file ;
	double size ;
	int type ;

};

class volumePropertiesThread : public QThread
{
public :
	volumePropertiesThread(void) ;
	void update(QString,QString,QString *) ;
private:
	void run() ;
	QString path ;
	QString mpoint ;
	QString *volProperty ;

};

#endif // ZULUCRYPTTHREADS_H
