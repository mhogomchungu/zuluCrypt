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

namespace lxqt {
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
	void pbkeyOption( void ) ;
	void pbMountPointPath( void ) ;
	void pbPluginEntryClicked( QAction * ) ;
	void keyTextChanged( QString ) ;
	void rbPlugIn( bool ) ;
	void rbKey( bool ) ;
	void rbKeyFile( bool ) ;
	void pbOpen( void ) ;
	void pbCancel( void ) ;
	void Plugin( void ) ;
	void KeyFile( void ) ;
	void cbMountReadOnlyStateChanged( int ) ;
	void fileManagerOpenStatus( int exitCode, int exitStatus,int startError ) ;
	void walletIsOpen( bool ) ;
	void getPassWord( QString ) ;
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
	QMenu * m_menu ;
	QTableWidget * m_table ;
	bool m_working ;
	bool m_autoOpenFolderOnMount ;
	lxqt::Wallet::Wallet * m_wallet ;
};

#endif // KEYDIALOG_H
