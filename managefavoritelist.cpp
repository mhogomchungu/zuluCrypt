#include "managefavoritelist.h"
#include "ui_managefavoritelist.h"

managefavoritelist::managefavoritelist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::managefavoritelist)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());
}

void managefavoritelist::ShowUI()
{
	this->show();
}

managefavoritelist::~managefavoritelist()
{
    delete ui;
}
