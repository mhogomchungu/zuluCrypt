#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
/*
 *
 *  Copyright (c) 2012
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

#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include <QDir>
#include <QIcon>
#include <QAction>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QFont>
#include <QCursor>

#include "keydialog.h"
#include "../zuluCrypt-gui/dialogmsg.h"
#include "../zuluCrypt-gui/userfont.h"
#include "../zuluCrypt-gui/tablewidget.h"
#include "mountpartition.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow( QWidget * parent = 0 );
	~MainWindow();

signals:
	void result( int,QString ) ;

private slots:
	void enterKeyPressed( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void pbMount( void ) ;
	void pbUmount( void ) ;
	void pbClose( void ) ;
	void slotUnmountComplete( int,QString ) ;
	void slotMountedList( QStringList,QStringList ) ;
	void slotcbReadOnly( void ) ;
	void slotCloseApplication( void ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;

private:
	Ui::MainWindow * m_ui ;

	QString m_action ;
	QString m_device ;

	bool m_working ;
	managepartitionthread * m_part ;
	void disableCommand( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e );
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	bool m_justMounted ;
};

#endif // MAINWINDOW_H
