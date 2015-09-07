/*
 *
 *  Copyright ( c ) 2015
 *  name : Francis Banyikwa
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


#ifndef HMAC_H
#define HMAC_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <functional>

class QCloseEvent ;
class QEvent ;

namespace Ui {
class hmac;
}

class hmac : public QDialog
{
	Q_OBJECT
public:
	explicit hmac( QDialog * parent,std::function< void( const QString& ) >,const QString& = QString() ) ;
	~hmac() ;
	void ShowUI() ;
	void HideUI() ;
private slots:
	void pbSetKey() ;
	void pbSelectKeyFile() ;
	void pbClose() ;
private:
	void enableAll() ;
	void disableAll() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::hmac * m_ui ;
	QString m_keyFile ;
	QString m_passphrase ;
	QString m_key = QString() ;
	std::function< void( const QString& ) > m_function ;
};

#endif // HMAC_H
