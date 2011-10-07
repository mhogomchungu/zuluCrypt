#include "managefavoritelist.h"
#include "ui_managefavoritelist.h"

managefavoritelist::managefavoritelist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::managefavoritelist)
{
    ui->setupUi(this);
}

managefavoritelist::~managefavoritelist()
{
    delete ui;
}
