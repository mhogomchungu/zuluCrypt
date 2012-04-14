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

#ifndef MANAGEDEVICENAMES_H
#define MANAGEDEVICENAMES_H

#include <QWidget>
#include <QString>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMenu>

#include "openpartition.h"
#include "miscfunctions.h"

namespace Ui {
class managedevicenames;
}

class managedevicenames : public QWidget
{
	Q_OBJECT
	
public:
	explicit managedevicenames(QWidget *parent = 0);
	~managedevicenames();
signals:
	void ShowPartitionUI(void);
	void HideUISignal(void);
public slots:
	void ShowUI(void);
	void HideUI(void);
	void PartitionEntry(QString);
private slots:
	void removeEntryFromFavoriteList(void);
	void add(void);
	void cancel(void);
	void deviceAddress(void);
	void fileAddress(void);
	void currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous );
	void itemClicked(QTableWidgetItem * current,bool);
	void itemClicked(QTableWidgetItem * current);
	void shortcutPressed(void);
	void devicePathTextChange(QString);
private:
	void HighlightRow(int,bool) ;
	void closeEvent(QCloseEvent *) ;
	void addEntries(QString,QString) ;
	Ui::managedevicenames * m_ui;
	QAction * m_ac ;
	/*
	  prototyped at miscfunctions.h
	  */
	UIMsg m_msg ;
};

#endif // MANAGEDEVICENAMES_H
