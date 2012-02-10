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

#ifndef OPEN_PARTITION_H
#define OPEN_PARTITION_H

#include <QCloseEvent>

#include "ui_openpartition.h"
#include "partitionproperties.h"
#include <QProcess>
#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QObject>
#include <QHeaderView>
#include <QFile>
#include <QThreadPool>
#include <QKeySequence>
#include "partitionproperties.h"
#include "../zuluCrypt-cli/executables.h"

class openpartition :  public QDialog
{
	Q_OBJECT
public:
	openpartition(QWidget *parent = 0);
	virtual ~openpartition();
signals :
	void HideUISignal(void);
	void clickedPartition(QString) ;
public slots:
	void tableEntryDoubleClicked(QTableWidgetItem *) ;
	void ShowAllPartitions(void);
	void HideUI(void);
	void ShowNonSystemPartitions(void) ;	
private slots:
	void EnterKeyPressed(void);
	void currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous );
	void partitionpropertiesThreadFinished();
	void partitionProperties(QStringList);
private:	
	void partitionList(QString,int);
	void closeEvent(QCloseEvent *) ;
	void HighlightRow(int, bool);	
	Ui::PartitionView * m_ui ;
	QAction * m_action ;
};

#endif
