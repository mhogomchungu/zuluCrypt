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

#include "contactinfo.h"
#include "ui_contactinfo.h"

#include <QObject>
#include <QCloseEvent>
#include <QEvent>

#include "utility.h"

const auto e = R"R(
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html><head><meta name="qrichtext" content="1" /><style type="text/css">
p, li { white-space: pre-wrap; }
</style></head><body style=" font-family:'%1'; font-size:%2pt; font-weight:%3; font-style:%4;">
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Name: Francis Banyikwa</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Email Address: mhogomchungu@gmail.com</p>
<p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><br /></p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">-----BEGIN PGP PUBLIC KEY BLOCK-----</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Version: GnuPG v1</p>
<p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><br /></p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">mQINBFX2u/UBEADAeacwlUtm2jKJBGyYIWD1h1EpUglAqh8bn7rHmdvs1IZWZEmE</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">xMoc1jujrGdBx6mguuKWY0VLGzgKhdrVv/rYsVbYlqH3Vz4mamBHOBjIcR317PVI</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">jV7xdGu76VQkIw1MiJR2TKADFYfe12mEMCTLG6lLAYcxaaIr6jPSZbaP+PUvAhQB</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">nCHqyA4uSlJnnzyfQuWnTxaedG2Qc0FAUHOnet/7wk3j/TTRXg7mFJVA+EXBwjUy</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">dJ2V3H/A1UDQ1FRhMw3L2Pjk4/Ast4f1yWmpJwNB8Zk9H8ezTFgnZW72Uo5wIGXN</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">mkA44skhWCB7cK7kCg/m9MCP6WCSVwqrSps5h1sM2ks32CyRikVXgK0Nbv3bB4+r</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">PY+ZEVvvts+Wj2KF+94OUUuAyklpb1M51JlScEcouV2oxDB0ijqqCaHugziLCifu</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">3gjtNacKP3lbBeGh4YjrXmrXpLOYhkrKLWCdv1rt2dcsehNhj7cRMkWoP4h28wbS</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">MQPgQW25w6QsDGU2uagsNg6It5hJGRuASRyqf/KddmUtSoaxmUwh/4aJo+R9oymQ</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">wYiHbxgx5OUTeCUv0Qjv2WPPYeYZkwxZbdYWiAPczC+PRXTGa00RgVBxOBMHcJl9</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">ZcxbC9QgUExPeh8ntK7rG+dpDj3GJ2qk8JUyWm7r2Ar3nCtfYl9LmV0JZQARAQAB</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">tClGcmFuY2lzIEJhbnlpa3dhIDxtaG9nb21jaHVuZ3VAZ21haWwuY29tPokCOAQT</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">AQIAIgUCVfa79QIbLwYLCQgHAwIGFQgCCQoLBBYCAwECHgECF4AACgkQPh84BCel</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">08rtahAAkdotOv7TfLJga/UgIU0AbBVEGgty41JFuILeUjA6TiHUCRDbAW73aiBw</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">poLe5Z3ZJH4UDEo3SWpilXks7PEBArP75EvG7XabPGQOjQ8ys8AcIpXHSUAvgYD/</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">OM6Bvcb1gNV81hqNl/Wh3mIvnk0hbyqecrRSm5a2cExJeHAuIdb991CI69oOgDBu</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">UcoI1FqBbcfuyBRAIl0WkvTcxHLGZLfRd/ZrSWB2FMt+Sj26V1VGGy4Jtb2S47Fw</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">xMkOOI6R5MH88I/jokNB7kCNUR33uNDVO2ugS4WpLStC0fX7GJY/RUMBH81OXKIy</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">dlPOjmpU/Ga3UgM6PeJJgrKjKSLuAFFynp0MogNU2o0bFrCR8PI0ZQxbN2h3X6Lb</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">uRqbqrNkPcBLtlRgp3uzia2/bzCSfZWcgmakW2yBxL6NSVMoDnYq5bXrWZOwE6n5</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">F2dHj40kZTa7nl4rDaAal791SIFWPKtt4g/CsMn5qxIZ3OQ0JD/ZsdSJ5BehNFdD</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">tJjPtk8fPdwY2t0JjaQeBYF04DMpT79f3S92fOqLYhIlkpxpdOEtp5hnjDuSHijP</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">PkkXWcTqDzm/B00EbMV/TL/2bTODHKVX4F2nDJfP9Qycu8wNc4FMS0knG+13sgte</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">bBIkAJ67KdfI0ex2/6YcrzgLlHSh0YypsvWb1L/usayLF9eRnRi5Ag0EVfa79QEQ</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">ANef8DH/z21+WdQuBs+/rSWMt3TnQbu+6ClZatBgQNb/Eedo6dY7v81NveLJLB/4</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">nwC3hJvmQCT/Lo8iiKr5/rq2ETfog8CVNGk4413X5SI5Y+WEMeimuJQOnPNemPEE</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">+aEyIwSWr2h30XzXJ89Q7chwj4M9minfnCGN8YndLljNWXXTuVCt30MvvdToxcw8</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">j0Gz+IdcdiTaq8r3S5ADk95hlVOztnL16agcX4YLZi1BpNBIle/ErCaHEmMdOtJl</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">vjH2I2metqOsd6i3TPgH/S2+U6HDPAL2G9bn68CNTOywwdGlAakNL91AAqxbbEBJ</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">4iNJ8mSoDrsq0PbFWYMR/BMR5tzRQnocl8jlZGEY0C32uidXzkK/NtrBm47WkedJ</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">VWyktUO29qB8MX2e1k4aJoZ4ODLZyXLybGVlIbLithonekH6X0H0UOMgHzeW7P6n</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">xn3gw5QALkmJ4w/C9g1n3EkoCCNbkSBzeE6ZJK535aSl8jQxcN07o8Y9MnB4qvyF</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">KNsv+82+ScdWOSi8M2KeHqS8t8PSiNgn9ckD5oyV7HoiQo90LrWvpmeBpS+jetAR</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">luTnwbfy7HwSjLerGPJtSul6q3wD93iLkljy9TOzaj+Wv7jZvhkhXhBWez/+x3Z8</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">ZFAHllKKJIWcdFr+AVADfmF4EgfllVpY3qTpV8mvXABBABEBAAGJBD4EGAECAAkF</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">AlX2u/UCGy4CKQkQPh84BCel08rBXSAEGQECAAYFAlX2u/UACgkQX5amBsT3KQV1</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">Rw//X3gGgdB1TNLuewZ3+y0kDBGSwwH/IxBTSn2IMoiwh+QdRQy/xmayQ4097iAk</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">3gHFtt49EPAl6ohWkKeGYRfq+3/hg5xKfmzwWMw5yRoHKZ8QhpJ5YpZBCRR0edeu</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">RNg6Qyx6cgr9mDIvB1k45DMpgNtMFfmSTigVvyTA+mfCGOdz2w9E6PRPTnddR7Gp</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">6HZvwvHBfshZuPGI1KTchfa07ibHazMpZKlw43V0vz0nz6wjmzjgGWsnbdbIT47+</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">mvAbFcG9lyDHoXlI02skvuqmImIvoKlRSTYdxsbZHGus0USFJXw0E7JkA42KFf5M</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">YbuwDD706Zbl672+yXxKkRKmp8c8zePL7PEwgADFHrxlrgaM+Qc8WDTCvy6FA6qd</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">LAkr9mOztO40sge92C9zmjFjwr3PJ7yxlIZh5wLAQD5tCFahKu1JdQG8DCTK6o6F</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">+oqTX1v4j2psfAxoQ0b0wkG1T/B8HSmp+oNZh/VRG7mTX6azqaIIPlyjKBN3w5xq</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">UVYqy2FNOyft5q5iuEeTcw4gxGGffDOieHdzoHcDKpmBzr0iJC52ajKC+PEnNxiA</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">CcvkpU34nbuPIGEbiL/TdWe88H7g0YJDkBqxmQxN5eJDSavi9MQ0fGP2khD1CDIk</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">dIC4qOr/GQ2NRqV6m5BsMDMVCX7kqn/xvACDc3XgoMWb+AXsIw/+Ka4NTdof18zP</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">5iGVMuLG6PBls6VcRNy8Ypr/spNJ1xHWl2f1yNyNX0MeZ8MswbWlE6ESZJ/PcVtD</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">dxjqZdhm3Z91CVbE9AxNhfSLsb3llLIK75J90Mo2Hx+GfLHCEbajpJfdfRENxIHb</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">HsE3QHDdDToY9NuCg40yOGdH+H/x+Qun9iuL7MuNZiibZfY1MD0ThlCUBmXDOdLV</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">l37Na2UvN3tBWsuOCSHqFZeivArfyv1Ez9ziVofmEPVQw3XkMVblxj8FvuIJzs8W</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">KrvShO4WLi69tTql4fVC9PzfhspgEphSo9wVuXuYj/CsxgF8wn6d27Oox3LR+Grs</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">w/oimllTMiNc7On5oZluZpHMyxMfgemsOs6XQXknbXA6lTYZo5JF7mUlf+4IoNlw</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">SdvbY7S+4enblKFmZ/ndsccFUOv3tpGBRhYu+7AszhZngB2P02vXBkdjhatK2FZH</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">zfdIrSpp0gs1J+xOdF/clQOPFn2oXdTcaBCUCp4d5rDszW4ItRyAgmhvMNbYZvVK</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">h/19Gl88Dj49v7SS83DqAtURjtXny2lo37uTAAZYgqMVeMff4Y8UJnYSomNwoBqf</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">pEH3RYyz2GwvaLpUvPUZvVr0x+dFSrHr3ze6v/q48OlmOTCkyukjYqsSOFgMIwBR</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">i/gJN6mX3AIU7HHEHnP5ty8mwbb8ROc=</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">=VDxL</p>
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">-----END PGP PUBLIC KEY BLOCK-----</p>
<p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><br /></p></body></html>

)R" ;

contactInfo::contactInfo( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::contactInfo )
{
	m_ui->setupUi( this ) ;

	const auto& font = parent->font() ;

	QString s = "normal" ;

	if( font.bold() ){

		s = "bold" ;

	}else if( font.italic() ){

		s = "italic" ;
	}

	QString f = QString( e ).arg( font.family(),QString::number( font.pointSize() ),QString::number( font.weight() ),s ) ;

	m_ui->textEdit->setText( f ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	this->setFont( font ) ;

	connect( m_ui->pushButton,SIGNAL( clicked( bool ) ),this,SLOT( pbOK() ) ) ;

	this->show() ;
}

contactInfo::~contactInfo()
{
	delete m_ui ;
}

void contactInfo::pbOK()
{
	this->hide() ;
	this->deleteLater() ;
}

void contactInfo::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbOK() ;
}

bool contactInfo::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbOK() ; } ) ;
}
