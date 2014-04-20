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

#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include <QDialog>
#include <QString>
#include <QMenu>

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class QTableWidget ;

namespace LxQt{
namespace Wallet {
class Wallet ;
}
}

#include "mainwindow.h"

namespace Ui {
class keyDialog;
}

class keyDialog : public QDialog
{
	Q_OBJECT
public:
	keyDialog( QWidget * parent,QTableWidget *,const QString& path,const QString& type,const QString& folderOpener,bool b = true ) ;
	~keyDialog() ;
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void mounted( QString ) ;
	void cryptoOpen( QString ) ;
	void cancel( void ) ;
public slots:
	void slotMountComplete( int,QString ) ;
private slots:
	void cbActicated( int ) ;
	void pbkeyOption( void ) ;
	void pbMountPointPath( void ) ;
	void pbPluginEntryClicked( QAction * ) ;
	void plugIn( void ) ;
	void key( void ) ;
	void keyFile( void ) ;
	void pbOpen( void ) ;
	void pbOptions( void ) ;
	void pbCancel( void ) ;
	void Plugin( void ) ;
	void KeyFile( void ) ;
	void cbMountReadOnlyStateChanged( int ) ;
	void fileManagerOpenStatus( int exitCode, int exitStatus,int startError ) ;
	void walletIsOpen( bool ) ;
	void getPassWord( QString ) ;
	void getPassWord( void ) ;
	void deviceOffSet( QString,QString ) ;
	void doAction( QAction * ) ;
	void showOffSetWindowOption( void ) ;
	void showFileSystemOptionWindow( void ) ;
private:
	void openVolume( void ) ;
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::keyDialog * m_ui ;
	QString m_path ;
	QString m_point ;
	QString m_folderOpener ;
	QString m_key ;
	QString m_deviceOffSet ;
	QString m_options ;
	QMenu * m_menu ;
	QMenu * m_menu_1 ;
	QTableWidget * m_table ;
	bool m_working ;
	bool m_autoOpenFolderOnMount ;
	LxQt::Wallet::Wallet * m_wallet ;
	enum{ Key = 0,keyfile = 1,plugin = 2 } ;
};

#endif // KEYDIALOG_H
