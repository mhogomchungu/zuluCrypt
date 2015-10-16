/*
 *
 *  Copyright (c) 2012-2015
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

#ifndef WALLETCONFIG_H
#define WALLETCONFIG_H

#include <QDialog>
#include <QString>
#include <QMap>

#include "lxqt_wallet/frontend/lxqt_wallet.h"
#include "utility.h"

class QCloseEvent ;
class QWidget ;
class QTableWidgetItem ;
class walletconfiginput ;

namespace Ui {
class walletconfig ;
}

class walletconfig : public QDialog
{
	Q_OBJECT
public:
	static walletconfig& instance( QWidget * parent )
	{
		return *( new walletconfig( parent ) ) ;
	}
	explicit walletconfig( QWidget * parent = 0 ) ;
	~walletconfig() ;
	void ShowUI( LxQt::Wallet::walletBackEnd ) ;
	void HideUI( void ) ;
signals:
	void HideUISignal( void ) ;
	void couldNotOpenWallet( void ) ;
private slots:
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked ( QTableWidgetItem * item ) ;
	void pbDelete( void ) ;
	void pbClose( void ) ;
	void pbAdd( void ) ;
	void walletIsOpen( bool ) ;
private:
	void enableAll( void ) ;
	void disableAll( void ) ;
	Ui::walletconfig * m_ui ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;

	LxQt::Wallet::Wallet * m_wallet ;
	int m_row ;
	QString m_volumeID ;
	QString m_comment ;
	QString m_key ;
};

#endif // KWALLETCONFIG_H
