#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QMainWindow>
#include <QTime>
#include <QMap>
#include <QtCharts>             //必须这么设置
#include <QJsonDocument>        //json

#include "datacenter.h"
#include "chart.h"
#include "chartview.h"
#include "dialog_setting_1.h"
#include "dialog_setting_2.h"
#include "udp_json_api.h"
#include "time_thread.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void    closeEvent(QCloseEvent *event);

public:

    explicit MainWindow(QWidget *parent = 0);

    QString To_Upper(quint8 data);

    int json_analysis(QByteArray& data);       //解析json数据

    void    createChart_speed_1(int index);   //创建图表
    void    createChart_dot();    //创建图表
    void    random_data_create();        //随机数据产生
    void    sleep(unsigned int msec);    //延时程序
    void    updata_line();               //更新绘线设置

    //暂时无用
    void    set_speed_time();
    void    set_default_speed();

    ~MainWindow();
signals:
    void random_data(unsigned int ID,double,double,QByteArray);
    void send_to_setting(bool flag);
    void send_udp_to_setting(bool flag);
    void send_API_udp_to_setting(bool flag);

private:

    Time_thread my_thread;         //计时线程
    QVector<QString> request{"GetCapabilities","InitPositioningClient"
                             ,"StartPositioning","ClosePositioningSession"};
    QVector<QString> client{"ab5456welkjs"};

    QHostAddress API_Addr;            //第三方api IP
    quint16 API_Port;                 //第三方api 端口
    QUdpSocket  *udp_API;             //第三方api接口
    udp_json_API my_json;             //json数据

    Ui::MainWindow *ui;
    DataCenter *dataCenter;             //数据中心
    Dialog_setting_1 *my_setting;       //设置窗口
    Dialog_setting_2 *my_help;          //关于窗口

    QHash <unsigned int ,int> vec_label;  //标签与序列的对应
    int i_count=0;                        //记录收到的标签个数
    bool new_data=false;                  //基站运作正常并有数据传入
    bool start_send_to_API=false;         //开始向第三方发送数据
    bool location_ready=false;            //定位初始化
    bool stations_deady=false;            //数据库信息已同步

    bool flag_1=false;                  //随机数产生循环条件
    bool Drawing_true=false;            //绘图开始确认
    bool save_data=false;               //是否存储
    bool simulation=true;               //以模拟模式绘制
    bool stop_accept=false;             //暂停接收

    QVector<QString>   data_save;            //测试信息输出，存数据使用
    QVector <bool> team1_point_true;         //绘制轨迹
    QVector <QString> vec_str_location;      //存储实时数据点

    QVector <QString> vec_speed_2;           //存储实时角速度点
    QVector <QString> vec_temp;              //存储实时温度点

    QVector <QScatterSeries*> team1_dot;     //每个标签对应点数组
    QVector <QColor> vec_color_team1;        //每个标签的颜色
    QVector <QList<float>> vec_speed_1;      //每个标签的加速度存储

    int index_big_1=0;                       //变化绘点大小所用记录上一个变大的点

    QLineSeries *series_speed;               //加速度曲线图

    QScatterSeries *curSeries_dot;       //当前序列_点图
    QPen    m_pen;                       //成员变量


    //暂时不用
    QList <float> default_speed;
    QVector<float> speed_time;

private slots:

    void udp_api_bind(quint16 port);      //绑定第三方api
    void close_API_udp_polt();
    void json_receive(udp_json_API m_json);
    void json_receive_and_send();
    //与信息确认相关的槽函数
    void accept_udp_massage(quint16 port);
    void close_udp_polt();

    //显示槽函数
    void show_data_1(QString str);
    void show_data_online_station(QString str);
    void updateChart(unsigned int ID,double x,double y,QByteArray health_str);   //接收数据并显示

    void show_feedback(bool flag,quint16 port);
    void analysis_message(QByteArray message,quint16 port);
    void heart_data();

    void on_pushButton_6_clicked();
    void on_cBoxAnimation_currentIndexChanged(int index);
    void on_cBoxTheme_currentIndexChanged(int index);
    void on_checkBox_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);
    void on_checkBox_5_clicked(bool checked);
    void on_pushButton_clicked();
    void on_action_set_triggered();
    void on_action_help_triggered();
    void on_combox_mine_currentIndexChanged(int index);
    void on_pushButton_2_clicked();
    void on_pushButton_8_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_pushButton_9_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_10_clicked();
};

#endif // MAINWINDOW_H
