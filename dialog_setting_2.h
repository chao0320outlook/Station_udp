#ifndef DIALOG_SETTING_2_H
#define DIALOG_SETTING_2_H

#include <QDialog>

namespace Ui
{
class Dialog_setting_2;
}

class Dialog_setting_2 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_setting_2(QWidget *parent = 0);
    ~Dialog_setting_2();

private:
    Ui::Dialog_setting_2 *ui;
};

#endif // DIALOG_SETTING_2_H
