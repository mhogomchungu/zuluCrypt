/*
 * copyright: 2013
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QString>
#include <QMessageBox>

namespace Ui {
class password_dialog;
}

class password_dialog : public QDialog
{
	Q_OBJECT
public:
	explicit password_dialog( QWidget * parent = 0 ) ;
	void ShowUI( const QString& walletName,const QString& applicationName ) ;
	void ShowUI( const QString& walletName ) ;
	void ShowUI( void ) ;
	void closeUIOnKeySend( void ) ;
	~password_dialog();
signals:
	void cancelled( void ) ;
	void password( QString ) ;
	void createWallet( bool ) ;
private slots:
	void pbSend( void ) ;
	void pbCancel( void ) ;
	void passwordIsCorrect( bool ) ;
	void pbOK( void ) ;
	void pbOK_2( void ) ;
private:
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::password_dialog * m_ui ;
	bool m_createWallet ;
	bool m_closeUIOnKeySend ;
	QString m_banner ;
};

#endif // PASSWORD_DIALOG_H
