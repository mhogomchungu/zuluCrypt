#ifndef ERASEDEVICE_H
#define ERASEDEVICE_H

#include <QWidget>
#include <QCloseEvent>
#include <QFileDialog>

#include "miscfunctions.h"
#include "erasedevicethread.h"
#include "openpartition.h"

namespace Ui {
class erasedevice;
}

class erasedevice : public QWidget
{
	Q_OBJECT	
public:
	explicit erasedevice(QWidget *parent = 0);
	~erasedevice();
	void ShowUI(void);
	void HideUI(void);
signals:
	void HideUISignal(void);
private slots:
	void enableAll(void);
	void disableAll(void);
	void pbStart(void);
	void pbCancel(void);
	void pbFile(void);
	void pbPartition(void);
	void setProgress(int);
	void setPath(QString);
	void threadExitStatus(int);
private:
	void run(void);
	void closeEvent(QCloseEvent *);
	Ui::erasedevice * m_ui;
	erasedevicethread * m_dt  ;
	bool m_cancelClicked ;
};

#endif // ERASEDEVICE_H
