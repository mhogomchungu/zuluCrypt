/*
 *
 *  Copyright (c) 2012
 *  name : Francis Banyikwa
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
#include <QStringList>

#include <QMenu>

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class QTableWidget ;

#include "mainwindow.h"
#include "volumeentryproperties.h"
#include "../zuluCrypt-gui/utility.h"

namespace Ui {
class keyDialog;
}

class keyDialog : public QDialog
{
	Q_OBJECT
public:
	keyDialog( QWidget * parent,QTableWidget *,const volumeEntryProperties& ) ;
	~keyDialog() ;
	void ShowUI( void ) ;
	void HideUI( void ) ;
signals:
	void mounted( QString ) ;
	void cryptoOpen( QString ) ;
	void cancel( void ) ;
	void openMountPoint( QString ) ;
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
	void deviceOffSet( QString,QString ) ;
	void doAction( QAction * ) ;
	void showOffSetWindowOption( void ) ;
	void showFileSystemOptionWindow( void ) ;
	void keys( QString key,QStringList keyFiles ) ;
	void tcryptCancelled( void ) ;
	void encfsMount( void ) ;
private :
	void tcryptGui( void ) ;
	void openVolume( void ) ;
	void enableAll( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::keyDialog * m_ui ;
	QString m_path ;
	QString m_point ;
	QString m_key ;
	QStringList m_keyFiles ;
	QString m_deviceOffSet ;
	QString m_options ;
	QMenu * m_menu ;
	QMenu * m_menu_1 ;
	QTableWidget * m_table ;
	utility::veraCryptWarning m_veraCryptWarning ;
	bool m_veraCryptVolume = false ;
	bool m_working ;
	bool m_volumeIsEncFs = false ;
	enum{ Key = 0,keyfile = 1,plugin = 2,tcryptKeys = 3 } ;
};

#endif // KEYDIALOG_H
