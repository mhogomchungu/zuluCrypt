#ifndef CREATEVOLUMEDIALOG_H
#define CREATEVOLUMEDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class CreateVolumeDialog;
}

class CreateVolumeDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CreateVolumeDialog(QString path,QWidget *parent = 0);
	void ShowUI(void);
	~CreateVolumeDialog();
signals:
	void dialogResult(int) ;
private slots:
	void pbYes(void);
	void pbNo(void);
private:
	Ui::CreateVolumeDialog * m_ui;
	int m_opt ;
	int m_opt_count ;
	QString m_path ;
};

#endif // CREATEVOLUMEDIALOG_H
