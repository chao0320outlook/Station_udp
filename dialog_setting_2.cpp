#include "dialog_setting_2.h"
#include "ui_dialog_setting_2.h"

Dialog_setting_2::Dialog_setting_2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_setting_2)
{
    ui->setupUi(this);
}

Dialog_setting_2::~Dialog_setting_2()
{
    delete ui;
}
