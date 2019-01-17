#ifndef MY_DIALOG_H
#define MY_DIALOG_H

#include <QDialog>
#include <QEvent>
#include <QVector>
#include <QVector3D>
#include <QMap>
#include "soccer_player.h"


namespace Ui
{
class my_Dialog;
}

class my_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit my_Dialog(QWidget *parent = 0);
    ~my_Dialog();
    void add_map(){size_a++;}

private:

    QVector3D vec={0.0f,0.0f,0.0f};

    QMap <unsigned int ,int> vec_player;        //index顺序（comboBox_2存储顺序 也是模型的存储顺序） 与  球员标签号码 的对应
    QVector<Soccer_player> player;              //运动员

    int size_a=0;                               //记录所需模型数
    QVector<QVector3D> vec_player_data;         //存储数据
    bool player_set=false;

    QVector <QString> vec_str;            //存储实时数据点

private slots:

    void show_data(unsigned int ID,QVector<int> speed_new,int temper);

    void updateplayer_data(QVector<Soccer_player> player_data);

    void update_player_location(unsigned int,double,double);

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void on_pushButton_5_pressed();

    void on_pushButton_5_released();

    void on_comboBox_activated(const QString &arg1);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_2_pressed();

    void on_pushButton_2_released();

    void on_pushButton_pressed();

    void on_pushButton_released();

    void on_pushButton_4_pressed();

    void on_pushButton_4_released();

    void show_camera();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_comboBox_2_currentIndexChanged(int index);


private:
    Ui::my_Dialog *ui;
};

#endif // MY_DIALOG_H


