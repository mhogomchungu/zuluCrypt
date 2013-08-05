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

#ifndef KWALLETCONFIG_H
#define KWALLETCONFIG_H

#include <QDialog>
#include <QCloseEvent>

#include "kwalletplugin.h"
#include "dialogmsg.h"
#include "openvolume.h"
#include "tablewidget.h"
#include "kwalletconfiginput.h"

#include <Qt>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMap>
#include <QFileDialog>

namespace Ui {
class kwalletconfig;
}

class kwalletconfig : public QDialog
{
	Q_OBJECT

public:
	explicit kwalletconfig( QWidget * parent = 0 );
	~kwalletconfig();
	void ReShowUI( void ) ;
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void HideUISignal( void ) ;
	void couldNotOpenWallet( void ) ;
private slots:
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous );
	void itemClicked ( QTableWidgetItem * item ) ;
	void pbDelete( void ) ;
	void pbClose( void ) ;
	void add( QString volumeID,QString comment,QString key ) ;
	void pbAdd( void ) ;
private:
	void failedToOpenWallet( void ) ;
	void ShowWalletEntries( void );
	Ui::kwalletconfig * m_ui;
	void closeEvent( QCloseEvent * ) ;
	kwalletplugin * m_wallet ;
	QMap < QString,QString > m_map ;
};

#endif // KWALLETCONFIG_H
