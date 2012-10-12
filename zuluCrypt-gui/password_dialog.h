/*
 *
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H

#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QCloseEvent>
#include <QTableWidget>
#include <stdlib.h>

#include "ui_password.h"
#include "openpartition.h"
#include "runinthread.h"
#include "miscfunctions.h"
#include "checkvolumetype.h"
#include "dialogmsg.h"
#include "kwalletplugin.h"
#include "plugin_path.h"
#include "tablewidget.h"
#include "../zuluCrypt-cli/constants.h"
#include "zuluoptions.h"
#include "socketsendkey.h"
#include "openvolumereadonly.h"
#include "openmountpointinfilemanager.h"

class passwordDialog :  public QDialog
{
	Q_OBJECT
public:
	passwordDialog( QTableWidget * table,QWidget * parent = 0 );
	virtual ~passwordDialog();
signals :
	void addItemToTable( QString,QString,QString ) ;
	void HideUISignal( void );
	void volumeOpened( QString,QString,passwordDialog * );
public slots:
	void HideUI( void );
	void ShowUI( void );
	void ShowUI( QString volumePath,QString mount_point ) ;
	void clickedPartitionOption( QString );
private slots :
	void pbPluginEntryClicked( QAction * ) ;
	void pbPlugin( void ) ;
	void keyTextChanged( QString ) ;
	void buttonOpenClicked( void ) ;
	void mount_point( void );
	void clickedPassPhraseFromFileButton( void ) ;
	void passphraseFromFileOption( void ) ;
	void passphraseOption( void ) ;
	void pluginOption( void ) ;
	void file_path( void  ) ;
	void threadfinished( int ) ;
	void mountPointPath( QString );
	void done( QString );
	void cbStateChanged( int );
private :
	void sendKey( QString sockpath ) ;
	QString getKeyFromKWallet( void ) ;
	void setDefaultOpenMode( void );
	void disableAll( void );
	void enableAll( void ) ;
	void closeEvent( QCloseEvent * ) ;
	void success( void );
	Ui::PasswordDialog * m_ui;
	bool m_isWindowClosable ;
	QTableWidget * m_table ;
	bool m_open_with_path ;
	QMenu * m_pluginMenu ;
	QWidget * m_parent ;
	QString m_key ;
};

#endif // PASSWORD_DIALOG_H

