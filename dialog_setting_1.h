#ifndef DIALOG_SETTING_1_H
#define DIALOG_SETTING_1_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDialog>
#include <QVector>
#include <QString>
#include <QHeaderView>

#include <QtSql>
#include <QDataWidgetMapper>
#include "udp_json_api.h"

namespace Ui
{
class Dialog_setting_1;
}

class Dialog_setting_1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_setting_1(QWidget *parent = 0);
    ~Dialog_setting_1();

    void openTable();
    void show_seetings();
    //界面信息设置
    void updata_seetings_to_db3();


signals:
    //发送基站位置到主函数
    void send_station(QVector<double> vec_station);
    //udp端口信息及打开信号  连接基站
    void send_udp_massage(quint16 port);
    //udp端口关闭信号
    void send_udp_massage_close();


    //udp端口信息及打开信号  连接第三方api
    void open_api_udp(quint16 port);
    //第三方apiudp端口关闭信号
    void send_udp_API_close();


    //第一次开启信号
    void fir_open();
    //发送数据库初始信息
    void send_my_api(udp_json_API m_api);

private slots:

    void open_udp_true(bool open_udp);
    void open_API_udp_true(bool open_udp);
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();


private:

    QSqlDatabase  DB;//数据库连接
    QSqlTableModel      *tabModel;  //数据模型
    QItemSelectionModel *theSelection; //选择模型

    udp_json_API my_api;


    bool data_base_open=false;
    bool first_open=true;           //是否第一次打开软件
    Ui::Dialog_setting_1 *ui;

    bool settings_true =false;       //各项基础信息确认

};

#endif // DIALOG_SETTING_1_H
