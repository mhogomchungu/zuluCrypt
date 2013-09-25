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

#include "password_dialog.h"
#include "ui_password_dialog.h"

lxqt::Wallet::password_dialog::password_dialog( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::password_dialog )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;

	connect( m_ui->pushButtonSend,SIGNAL( clicked() ),this,SLOT( pbSend() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pushButtonOK_2,SIGNAL( clicked() ),this,SLOT( pbOK_2() ) ) ;

	m_ui->pushButtonOK_2->setVisible( false ) ;
	m_ui->textEdit_2->setVisible( false ) ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->pushButtonOK->setVisible( false ) ;

	m_closeUIOnKeySend = false ;
}

void lxqt::Wallet::password_dialog::ShowUI( const QString& walletName,const QString& applicationName )
{
	m_createWallet = false ;
	m_banner = m_ui->textEdit->toHtml().arg( applicationName ).arg( walletName ) ;
	m_ui->labelWalletDoesNotExist->setVisible( false ) ;
	m_ui->labelHeader->setText( m_banner ) ;
	this->show() ;
}

void lxqt::Wallet::password_dialog::ShowUI( const QString& walletName )
{
	m_createWallet = true ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->labelWalletDoesNotExist->setVisible( true ) ;
	m_ui->labelWalletDoesNotExist->setText( m_ui->textEdit_2->toHtml().arg( walletName ) ) ;
	m_ui->labelHeader->setVisible( false ) ;
	m_ui->lineEditKey->setVisible( false ) ;
	m_ui->pushButtonSend->setText( tr( "yes" ) ) ;
	m_ui->pushButtonCancel->setText( tr( "no" ) ) ;
	this->show() ;
}

void lxqt::Wallet::password_dialog::ShowUI()
{
	m_createWallet = true ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->labelWalletDoesNotExist->setVisible( true ) ;
	m_ui->labelWalletDoesNotExist->setText( tr( "wallet could not be opened with the presented key" ) ) ;
	m_ui->labelHeader->setVisible( false ) ;
	m_ui->lineEditKey->setVisible( false ) ;
	m_ui->pushButtonSend->setVisible( false ) ;
	m_ui->pushButtonCancel->setVisible( false ) ;
	connect( m_ui->pushButtonOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	m_ui->pushButtonOK->setVisible( true ) ;
	this->show() ;
}

void lxqt::Wallet::password_dialog::closeUIOnKeySend()
{
	m_closeUIOnKeySend = true ;
}

lxqt::Wallet::password_dialog::~password_dialog()
{
	delete m_ui;
}

void lxqt::Wallet::password_dialog::pbSend()
{
	if( m_createWallet ){
		emit createWallet( true ) ;
		this->HideUI() ;
	}else{
		m_ui->textEdit->setEnabled( false ) ;
		m_ui->labelHeader->setEnabled( true ) ;
		m_ui->lineEditKey->setEnabled( false ) ;
		m_ui->lineEditKey->setEnabled( false ) ;
		m_ui->pushButtonSend->setEnabled( false ) ;
		m_ui->pushButtonCancel->setEnabled( false ) ;
		m_ui->pushButtonOK->setEnabled( false ) ;
		m_ui->pushButtonOK_2->setEnabled( true ) ;
		emit password( m_ui->lineEditKey->text() ) ;
	}
	if( m_closeUIOnKeySend ){
		this->HideUI() ;
	}
}

void lxqt::Wallet::password_dialog::pbCancel()
{
	if( m_createWallet ){
		emit createWallet( false ) ;
		this->HideUI() ;
	}else{
		emit cancelled() ;
		this->HideUI() ;
	}
}

void lxqt::Wallet::password_dialog::passwordIsCorrect( bool correctPassword )
{
	if( correctPassword ){
		this->HideUI() ;
	}else{
		m_ui->labelHeader->setText( tr( "wallet could not be opened with the presented key" ) ) ;
		m_ui->textEdit->setVisible( false ) ;
		m_ui->labelWalletDoesNotExist->setVisible( false ) ;
		m_ui->labelHeader->setVisible( true ) ;
		m_ui->lineEditKey->setVisible( true ) ;
		m_ui->lineEditKey->setEnabled( false ) ;
		m_ui->pushButtonSend->setVisible( false ) ;
		m_ui->pushButtonCancel->setVisible( false ) ;
		m_ui->pushButtonOK->setVisible( false ) ;
		m_ui->pushButtonOK_2->setVisible( true ) ;
	}
}

void lxqt::Wallet::password_dialog::pbOK()
{
	this->HideUI() ;
}

void lxqt::Wallet::password_dialog::pbOK_2()
{
	m_ui->labelHeader->setText( m_banner ) ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->labelWalletDoesNotExist->setVisible( false ) ;
	m_ui->labelHeader->setVisible( true ) ;
	m_ui->lineEditKey->setVisible( true ) ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->pushButtonSend->setVisible( true ) ;
	m_ui->pushButtonCancel->setVisible( true ) ;
	m_ui->pushButtonOK->setVisible( false ) ;
	m_ui->pushButtonOK_2->setVisible( false ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setFocus() ;
	m_ui->textEdit->setEnabled( true ) ;
	m_ui->labelHeader->setEnabled( true ) ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->pushButtonSend->setEnabled( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonOK->setEnabled( true ) ;
	m_ui->pushButtonOK_2->setEnabled( true ) ;
}

void lxqt::Wallet::password_dialog::HideUI()
{
	this->deleteLater() ;
}

void lxqt::Wallet::password_dialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
