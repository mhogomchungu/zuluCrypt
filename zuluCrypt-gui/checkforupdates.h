/*
 *
 *  Copyright (c) 2015
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

#ifndef CHECKFORUPDATES_H
#define CHECKFORUPDATES_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QString>
#include <QByteArray>

#include "networkAccessManager.hpp"

#include <utility>
#include <array>

class checkForUpdates : public QObject
{
	Q_OBJECT
public:
	static void instance( QWidget *,const QString& ) ;
	static void instance( QWidget * ) ;

	checkForUpdates( QWidget * widget,bool autocheck ) ;

private slots:
	void timeOut() ;
private:
	void showResult( const QString& ) ;
	QString parseResult( const QByteArray& ) ;

	QWidget * m_widget ;

	QNetworkReply * m_networkReply ;
	NetworkAccessManager m_network ;

	QTimer m_timer ;

	bool m_autocheck ;
} ;

#endif // CHECKFORUPDATES_H
