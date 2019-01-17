#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>
#include <QVector>
#include <QList>
#include <QHash>
#include <QSet>
#include <QUdpSocket>


#include "matlib.h"
#include "label_mine.h"
#include "kalman.h"
#include "time_thread.h"


typedef  struct
{
    double B;
    double u;
    double K[8];
    double z[2];
    double P[9][16];
    double x[9][4];
    double A[16];
    double Q[16];
    double H[8];
    double R[4];
} Kf_struct;


struct NewPoint
{
    double x;     //x坐标
    double y;     //y坐标
};

class DataCenter : public QObject
{
    Q_OBJECT
public:

    explicit DataCenter(QObject *parent=0);
    ~DataCenter();

    QByteArray   datagram;
    QVector<quint8> receiveBuffer;
    //设置基站信息
    void set_Base_station_1_x(double x1){Base_station[0].x=x1;}
    void set_Base_station_1_y(double y1){Base_station[0].y=y1;}

    void set_Base_station_2_x(double x1){Base_station[1].x=x1;}
    void set_Base_station_2_y(double y1){Base_station[1].y=y1;}

    void set_Base_station_3_x(double x1){Base_station[2].x=x1;}
    void set_Base_station_3_y(double y1){Base_station[2].y=y1;}

    void set_Base_station_4_x(double x1){Base_station[3].x=x1;}
    void set_Base_station_4_y(double y1){Base_station[3].y=y1;}

    void set_center_point()
    {
        Base_station[4].y=(Base_station[1].y-Base_station[0].y)/2.0;
        Base_station[4].x=(Base_station[2].x-Base_station[0].x)/2.0;
    }

    void send_to_station(quint16 port_1,QByteArray ch);

    //数据解析
    void dataAnalysisSerial(const QByteArray& data);//数据解码
    void Taylor(const QVector<unsigned int>& vec);
    void Taylor_test(const QVector<unsigned int>& vec);
    void Taylor_new(const QVector<unsigned int>& vec);
    void kf_settings(unsigned int ID);
    void kf_update(unsigned int ID);
    //void kf_update_new(unsigned int ID);
    double distance_sqr(NewPoint const* a, NewPoint const* b);
    QString To_Upper(quint8 data);
    void set_labels_Loction(const QVector<unsigned int>& vec ,const QByteArray &str);
    void data_analyze(const QByteArray& data);

signals://信号
    void new_feedback(bool flag,quint16 port);
    void new_message(QByteArray str_1,quint16 port);
    void new_on_line_station(QString str);
    void new_data_test(QString str);
    void new_wrong_data_test(QString str);
    void send_new_data(unsigned int ID,double x,double y,QByteArray str_1);

public slots:
    //udp绑定
    bool bind_udp(quint16 port);
    //udp解绑
    void close_udp();
    void send_sation_state();
private slots:
    //数据接收
    void onSocketReadyRead();   //udp通讯数据接收

private:

    QHostAddress    peerAddr;
    On_line_thread station_check_thread;    //每隔两秒发送基站在线情况的线程

    QString sta_old,sta_old_2;
    Label_S my_labels;
    QUdpSocket  *udpSocket;
    QString udp_IP,udp_IP_2;


    QHash<int,unsigned int> num_sta;       //基站id与序列的对应
    QHash<int,int> sta_count;              //各基站收到信息次数统计
    QSet<int>  online_station;             //在线基站名单

    QHash<unsigned int,NewPoint> Base_stations;    //后续版本使用
    NewPoint Base_station[5];             //暂时用

    NewPoint x0y0={50,0};                 //实时位置信息

    Kf_struct kf_params = { {0.0}, { 0.0},
                            {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} , {0.0 , 0.0},
                            {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}},
                            {{0.0, 0.0 ,0.0 ,0.0}},
                            {1,0,1,0,  0,1,0,1,  0,0,1,0,  0,0,0,1},
                            {0.001,0,0,0,  0,0.001,0,0,0,  0,0.001,0,0,  0,0,0.001},
                            {1,0,0,0,  0,1,0,0},
                            {2,0,0,2}
                          };
    //Kalman KM_data;

    QHash<unsigned int,QList<NewPoint> > lasts_x_y_pos;  //记录上一次定位值
    QHash<unsigned int,QVector<unsigned int>> nums;  //记录近三次的序列


};

#endif // DATACENTER_H
