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

#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

lxqt::Wallet::changePassWordDialog::changePassWordDialog( QWidget * parent,const QString& walletName,const QString& applicationName ):
	QDialog( parent ),m_ui( new Ui::changePassWordDialog ),m_wallet( 0 ),m_walletName( walletName ),
	m_applicationName( applicationName )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;

	m_ui->pushButtonOK->setVisible( false ) ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->textEdit_2->setVisible( false ) ;

	m_walletPassWordChanged = false ;
}

void lxqt::Wallet::changePassWordDialog::ShowUI()
{
	m_banner = m_ui->textEdit->toHtml().arg( m_applicationName ).arg( m_walletName ) ;
	m_ui->label->setText( m_banner ) ;
	connect( m_ui->pushButtonChange,SIGNAL( clicked() ),this,SLOT( change() ) ) ;
	connect( m_ui->pushButtonOK,SIGNAL( clicked() ),this,SLOT( ok() ) ) ;
	this->show() ;
}

void lxqt::Wallet::changePassWordDialog::HideUI()
{
	emit walletpassWordChanged( m_walletPassWordChanged ) ;
	this->hide() ;
	this->deleteLater() ;
}

void lxqt::Wallet::changePassWordDialog::ShowUI_1()
{
	this->setWindowTitle( tr( "create a new wallet" ) ) ;

	m_ui->pushButtonChange->setText( tr( "create" ) ) ;

	connect( m_ui->pushButtonChange,SIGNAL( clicked() ),this,SLOT( create() ) ) ;
	connect( m_ui->pushButtonOK,SIGNAL( clicked() ),this,SLOT( ok_1() ) ) ;

	m_banner = m_ui->textEdit_2->toHtml().arg( m_applicationName ).arg( m_walletName ) ;
	m_ui->label->setText( m_banner ) ;

	m_ui->label_2->setEnabled( false ) ;
	m_ui->lineEditCurrentPassWord->setEnabled( false ) ;
	this->show() ;
}

lxqt::Wallet::changePassWordDialog::~changePassWordDialog()
{
	delete m_ui ;
	lxqt_wallet_close( &m_wallet ) ;
}

void lxqt::Wallet::changePassWordDialog::create()
{
	if( m_ui->lineEditNewPassWord->text() == m_ui->lineEditNewPassWord_2->text() ){
		emit password( m_ui->lineEditNewPassWord->text(),true ) ;
		this->HideUI() ;
	}else{
		m_ui->label->setText( tr( "passwords do not match" ) ) ;
		m_ui->pushButtonOK->setVisible( true ) ;
		m_ui->pushButtonCancel->setVisible( false ) ;
		m_ui->pushButtonChange->setVisible( false ) ;

		m_ui->lineEditCurrentPassWord->setEnabled( false ) ;
		m_ui->lineEditNewPassWord->setEnabled( false ) ;
		m_ui->lineEditNewPassWord_2->setEnabled( false ) ;
		m_ui->label->setEnabled( true ) ;
		m_ui->label_2->setEnabled( false ) ;
		m_ui->label_2->setEnabled( false ) ;
		m_ui->label_3->setEnabled( false ) ;
		m_ui->label_4->setEnabled( false ) ;
	}
}

void lxqt::Wallet::changePassWordDialog::change()
{
	m_ui->lineEditCurrentPassWord->setEnabled( false ) ;
	m_ui->lineEditNewPassWord->setEnabled( false ) ;
	m_ui->lineEditNewPassWord_2->setEnabled( false ) ;
	m_ui->pushButtonChange->setEnabled( false ) ;
	m_ui->pushButtonCancel->setEnabled( false ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label_3->setEnabled( false ) ;
	m_ui->label_4->setEnabled( false ) ;

	if( m_ui->lineEditNewPassWord->text() == m_ui->lineEditNewPassWord_2->text() ){
		QString password = m_ui->lineEditCurrentPassWord->text() ;
		lxqt::Wallet::Task * t = new lxqt::Wallet::Task( &m_wallet,password,m_walletName,m_applicationName ) ;
		if( t ){
			connect( t,SIGNAL( walletOpened( bool ) ),this,SLOT( taskResult( bool ) ) ) ;
			t->start( lxqt::Wallet::Task::openInternal ) ;
		}else{
			this->taskResult( false ) ;
		}
	}else{
		m_ui->label->setText( tr( "new passwords do not match" ) ) ;
		m_ui->pushButtonOK->setVisible( true ) ;
		m_ui->pushButtonCancel->setVisible( false ) ;
		m_ui->pushButtonChange->setVisible( false ) ;
		m_ui->pushButtonOK->setFocus() ;
	}
}

void lxqt::Wallet::changePassWordDialog::cancel()
{
	emit password( m_ui->lineEditNewPassWord->text(),false ) ;
	this->HideUI() ;
}

void lxqt::Wallet::changePassWordDialog::ok()
{
	m_ui->lineEditCurrentPassWord->setEnabled( true ) ;
	m_ui->lineEditCurrentPassWord->clear() ;
	m_ui->lineEditCurrentPassWord->setFocus() ;
	m_ui->lineEditNewPassWord->setEnabled( true ) ;
	m_ui->lineEditNewPassWord_2->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->label_4->setEnabled( true ) ;
	m_ui->pushButtonOK->setVisible( false ) ;
	m_ui->pushButtonCancel->setVisible( true ) ;
	m_ui->pushButtonChange->setVisible ( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonChange->setEnabled( true ) ;
	m_ui->label->setText( m_banner ) ;
}

void lxqt::Wallet::changePassWordDialog::ok_1()
{
	m_ui->lineEditCurrentPassWord->setEnabled( false ) ;
	m_ui->lineEditNewPassWord->setEnabled( true ) ;
	m_ui->lineEditNewPassWord->setFocus() ;
	m_ui->lineEditNewPassWord_2->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->label_4->setEnabled( true ) ;
	m_ui->pushButtonOK->setVisible( false ) ;
	m_ui->pushButtonCancel->setVisible( true ) ;
	m_ui->pushButtonChange->setVisible( true ) ;
	m_ui->pushButtonCancel->setEnabled( true ) ;
	m_ui->pushButtonChange->setEnabled( true ) ;
	m_ui->label->setText( m_banner ) ;
}

void lxqt::Wallet::changePassWordDialog::taskResult( bool opened )
{
	if( opened ){
		QString new_password = m_ui->lineEditNewPassWord->text() ;
		lxqt_wallet_error r = lxqt_wallet_change_wallet_password( m_wallet,new_password.toLatin1().constData(),new_password.size() ) ;
		if( r == lxqt_wallet_no_error ){
			m_walletPassWordChanged = true ;
			this->HideUI() ;
		}else{
			m_ui->pushButtonChange->setEnabled( true ) ;
			m_ui->pushButtonCancel->setEnabled( true ) ;
			m_ui->label->setText( tr( "wallet password could not be changed" ) ) ;
			m_ui->pushButtonCancel->setVisible( false ) ;
			m_ui->pushButtonChange->setVisible( false ) ;
			m_ui->pushButtonOK->setVisible( true ) ;
			m_ui->pushButtonOK->setFocus() ;
		}
	}else{
		m_ui->pushButtonChange->setEnabled( true ) ;
		m_ui->pushButtonCancel->setEnabled( true ) ;
		m_ui->label->setText( tr( "wallet could not be opened with the presented key" ) ) ;
		m_ui->pushButtonCancel->setVisible( false ) ;
		m_ui->pushButtonChange->setVisible( false ) ;
		m_ui->pushButtonOK->setVisible( true ) ;
		m_ui->pushButtonOK->setFocus() ;
	}
}

void lxqt::Wallet::changePassWordDialog::closeEvent( QCloseEvent * e )
{
	emit password( m_ui->lineEditNewPassWord->text(),false ) ;
	e->ignore() ;
	this->HideUI() ;
}
