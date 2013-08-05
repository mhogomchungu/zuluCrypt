#ifndef KWALLETCONFIGINPUT_H
#define KWALLETCONFIGINPUT_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QCloseEvent>

#include "dialogmsg.h"
#include "utility.h"
#include "openvolume.h"

namespace Ui {
class kwalletconfiginput;
}

class kwalletconfiginput : public QDialog
{
	Q_OBJECT
public:
	explicit kwalletconfiginput( QWidget * parent = 0 ) ;
	~kwalletconfiginput();
	void ShowUI( void ) ;
signals:
	void add( QString volumeId,QString comment,QString key ) ;
public slots:
	void HideUI( void ) ;
private slots:
	void pbAdd( void ) ;
	void pbImageFilePath( void ) ;
	void pbVolumePath( void ) ;
	void setvolumeID( QString ) ;
private:
	void focus( void ) ;
	Ui::kwalletconfiginput * m_ui ;
	void closeEvent( QCloseEvent * ) ;
};

#endif // KWALLETCONFIGINPUT_H
