#include "datacenter.h"
#include <QVector>
#include <cmath>
#include <Eigen/Dense>

#include "label_mine.h"
using namespace Eigen;
using namespace std;

//date[2]为序列  date[3][4][5][6][7][8]为基站地址   date[14][15][16][17][18][19]为标签地址  date[26]--data[73] 附加信息

static const int N=4;
static const int FEEDBACK_SIZE=5;       //命令回馈帧长度
static const int BUFFER_SIZE=76;       //定位帧长度
static const int RSTART_SIZE=79;        //重置帧长度
static const int MESSAGE_SIZE=23;       //下位机信息帧长度

//定位帧解析
static const int Location_num=2;       //序列位置
static const int Location_sta=3;       //基站地址
static const int Location_label=14;    //标签地址
static const int Location_labeltime=20;    //标签时钟
static const int Location_signal=25;   //信号强度地址
static const int Location_health_front=26;  //健康信息起点
static const int Location_health_back=74;   //健康信息尾后一点

static const double CHANGGE_M=0.00468;     //   15.6*3/10000


DataCenter::DataCenter(QObject *parent) :
    QObject(parent)
{
    QString str="192.168.2.1";
    QHostAddress targetAddr(str);
    peerAddr=targetAddr;
    udpSocket=new QUdpSocket(this);   //udp接收
    connect(&station_check_thread,SIGNAL(send_on_line_station()),this,SLOT(send_sation_state()));  //udp接收与数据处理绑定
}
DataCenter::~DataCenter()
{
    udpSocket->close();
    station_check_thread.stopThread();
    station_check_thread.quit();
}

//udp绑定端口
bool DataCenter::bind_udp(quint16 port)
{
    if (udpSocket->bind(port))//绑定端口成功
    {
        connect(this->udpSocket,SIGNAL(readyRead()),this,SLOT(onSocketReadyRead()));  //udp接收与数据处理绑定
        station_check_thread.start();
        return true;
    }
    else
        return false;
}

//发送基站在线情况
void DataCenter::send_sation_state()
{
    QString str;
    if(!online_station.isEmpty())
    {
        for(const auto &i:online_station)
            str+=QString::number(i,10)+" ";
        str+="号基站";
        emit new_on_line_station(str);
        online_station.clear();
    }
    else
    {
         str="无";
         emit new_on_line_station(str);
    }
}

//dup接触绑定
void DataCenter::close_udp()
{
    while(udpSocket->isValid())     //加循环是因为若正在传递数据，则不能正常关闭
    {
        udpSocket->abort();
    }
    //内部数据清空
    online_station.clear();
    my_labels.clear();
    sta_count.clear();
    num_sta.clear();

    //关闭计时线程
    station_check_thread.quit();
    QString str="无";
    emit new_on_line_station(str);

}

//udp读取收到的数据报
void DataCenter::onSocketReadyRead()
{
    while(udpSocket->hasPendingDatagrams())
    {
        datagram.resize(udpSocket->pendingDatagramSize());
        quint16 peerPort;
        udpSocket->readDatagram(datagram.data(),datagram.size(),&peerAddr,&peerPort);
        auto data_size=datagram.size();

        //若为单帧定位信息
        if(data_size==BUFFER_SIZE)
        {
            dataAnalysisSerial(datagram);
        }
        //若为命令信息
        else if(data_size<80)
        {
            switch (data_size)
            {
            case FEEDBACK_SIZE:                     //命令回馈帧
                if(datagram[2]==0)
                    emit new_feedback(true,peerPort);
                else
                    emit new_feedback(false,peerPort);
                break;
            case MESSAGE_SIZE:
                //若为成功
                if(datagram[2]==0)
                {
                     QByteArray result;
                     for(int i=3;i!=21;++i)
                         result.push_back(datagram[i]);
                     emit new_message(result,peerPort);
                }
                else
                    emit new_feedback(false,peerPort);
                break;
            case RSTART_SIZE:
                break;
            default:
                break;
            }
        }
        //若为多帧定位信息
        else
        {
            auto flag=data_size%BUFFER_SIZE;
            if(flag==0)
            {
                auto num=data_size/BUFFER_SIZE;
                for(int i=0;i!=num;++i)
                {
                    QByteArray arr;
                    for(int j=0;j!=BUFFER_SIZE;++j)
                        arr+=datagram[BUFFER_SIZE*i+j];
                    dataAnalysisSerial(arr);
                }
            }
        }
        //若数据报为多帧叠加
        datagram.clear();
    }
}

//数据分组及整合
void DataCenter::dataAnalysisSerial(const QByteArray& data)
{
    int num=(quint8)data[Location_num];            //序列
    int station_num=(quint8)data[Location_sta];    //基站序列
    int lablel_num=(quint8)data[Location_label];   //标签序列

    //基站次数 map set 更新
    sta_count[station_num]++;
    online_station.insert(station_num);

    //若不存在该标签
    auto label_location=my_labels.find(lablel_num);   //标签存储序列
    if(label_location==-1)
    {
        Station_massage sta;
        sta.add(data);
        Label_mine label;
        label.add(station_num,sta);
        my_labels.add_new_label(label,lablel_num);
    }
    else
    {
        auto station_location=my_labels.find_station(label_location,station_num); //基站存储序列
        //若标签内无该基站
        if(station_location==-1)
        {
            Station_massage sta;
            sta.add(data);
            bool new_data=my_labels.add_new_station(sta,label_location,station_num);
            if(new_data)
            {
                const QByteArray& result= my_labels.new_location(label_location);
                //调用定位函数
                data_analyze(result);
            }
        }

        //若均存在
        else
        {
            bool new_data=my_labels.add_new_data(data,label_location,station_location);
            //满足定位要求
            if(new_data)
            {
                QByteArray result= my_labels.new_location(label_location);
                //调用定位函数
                data_analyze(result);
            }
        }
    }
}

//数据分析
void DataCenter::data_analyze(const QByteArray& data)
{
    /* data_labels 中依次为   标签ID[0]  序列[1]
        1号基站基站ID[2] 时间[3]  1号基站基站ID[4] 时间[5]
        3号基站基站ID[6] 时间[7]  4号基站基站ID[8] 时间[9]   标签时钟[10]*/
    QVector<unsigned int> data_labels;
    data_labels.push_back(data[Location_label]);
    data_labels.push_back(data[Location_num]);

    //存储基站ID与时间戳
    for(int i=Location_sta;i<BUFFER_SIZE*4;i=i+BUFFER_SIZE)
    {
        data_labels.push_back(data[i]);       //基站id
//        QByteArray sta_time;
//        for(int j=6;j!=10;++j)
//            sta_time.push_front(data[i+j]);
//        unsigned int data1=sta_time.toHex().toUInt();

        unsigned int data1=static_cast<quint8>(data[i+6])+
                        (static_cast<quint8>(data[i+7]))*256+
                        (static_cast<quint8>(data[i+8]))*256*256+
                        (static_cast<quint8>(data[i+9]))*256*256*256;  //基站时钟
        data_labels.push_back(data1);
    }

    //标签时钟
//    QByteArray sta_time;
//    for(int i=Location_labeltime;i!=Location_signal;++i)
//        sta_time.push_front(data[i+j]);
//    unsigned int data1=sta_time.toHex().toUInt();

    unsigned int data1=static_cast<quint8>(data[Location_labeltime])+
                    (static_cast<quint8>(data[Location_labeltime+1]))*256+
                    (static_cast<quint8>(data[Location_labeltime+2]))*256*256+
                    (static_cast<quint8>(data[Location_labeltime+3]))*256*256*256;  //基站时钟
    data_labels.push_back(data1);

    QByteArray health_data;
    //健康信息
    for(int i=Location_health_front;i!=Location_health_back;++i)
    {
        health_data.push_back(data[i]);
    }

    //测试用信息
    QString str_all;
    str_all+=To_Upper(static_cast<quint8>(data[Location_label]))+" ";  //标签
    str_all+=To_Upper(static_cast<quint8>(data[Location_num]))+" ";    //序列
    for(int i=Location_sta,j=3;i<BUFFER_SIZE*4;i=i+BUFFER_SIZE,j+=2)
    {
        str_all+=To_Upper(static_cast<quint8>(data[i]))+" ";             //基站id
        str_all+=QString::number(data_labels[j], 10)+" ";   //添加计算后的时间戳
    }
    //添加原始时间戳
    for(int i=Location_num;i<BUFFER_SIZE*4;i=i+BUFFER_SIZE)
    {
        str_all+=To_Upper((quint8)data[i])+"";   //序列

        str_all+=To_Upper((quint8)data[i+7])+"";
        str_all+=To_Upper((quint8)data[i+8])+"";
        str_all+=To_Upper((quint8)data[i+9])+"";
        str_all+=To_Upper((quint8)data[i+10])+" ";
    }
    //添加时间差
    auto time_1=(static_cast<int>(data_labels[5]-data_labels[3]))*CHANGGE_M;
    auto time_2=(static_cast<int>(data_labels[7]-data_labels[3]))*CHANGGE_M;
    auto time_3=(static_cast<int>(data_labels[9]-data_labels[3]))*CHANGGE_M;
    str_all+=QString::number(time_1,'g',4)+"        "+QString::number(time_2,'g',4)+"       "+QString::number(time_3,'g',4);
    sta_old=str_all;

    //添加最原始的时间戳
//    QString str_1;
//    for(int i=0;i<BUFFER_SIZE*4;++i)
//    {
//        if(i%76==0)
//            str_1+="\n";
//        str_1+=To_Upper((quint8)data[i])+"";
//    }
//    str_all+=str_1;

    //emit new_data_test(str_all);

//    if(abs(time_1)>5000||abs(time_2)>5000||abs(time_3)>5000)
//    {
//        emit new_wrong_data_test(str_all);
//    }

    set_labels_Loction(data_labels,health_data);
}
//定位函数
void DataCenter::set_labels_Loction(const QVector<unsigned int>& vec,const QByteArray &str)
{
    /* vec  中依次为   标签ID[0]  序列[1]
        1号基站基站ID[2] 时间[3]  1号基站基站ID[4] 时间[5]
        3号基站基站ID[6] 时间[7]  4号基站基站ID[8] 时间[9]   标签时钟[10]*/

    //进行基站与序列顺序的对应
    for(int i=0;i!=4;++i)
        num_sta[i]=vec[2+i*2];

    unsigned int ID=vec[0];

    //添加该标签的序列集
    nums[ID].push_back(vec[1]);
    if(nums[ID].size()==4)
        nums[ID].pop_front();

    //Taylor_new(vec);
    Taylor(vec);

    //Taylor_test(vec);
    kf_settings(ID);

    // 发送测试用信息
    QString str_test="("+ QString::number(x0y0.x, 10, 3)+","+QString::number(x0y0.y, 10, 3)+")";
    sta_old+=" "+str_test;
    QString str_count;
    //显示基站计数
    for( auto i=sta_count.cbegin();i!=sta_count.end();++i)
    {
        str_count+="["+QString::number(i.key(),10)+","+QString::number(i.value(),10)+"]";
    }
    sta_old+=str_count;
    emit new_data_test(sta_old);

    emit send_new_data(ID,x0y0.x,x0y0.y,str);    //发送ID 位置 以及附加信息

}

void DataCenter::Taylor(const QVector<unsigned int>& vec)
{
    /* vec  中依次为   标签ID[0]  序列[1]
        1号基站基站ID[2] 时间[3]  1号基站基站ID[4] 时间[5]
        3号基站基站ID[6] 时间[7]  4号基站基站ID[8] 时间[9]   标签时钟[10]*/

    auto ID=vec[0];

    int n,i;
    double fx0y0[N];      //点到4基站之间的距离

    double A_juzhen[2*N-1],L_juzhen[N-1];
    double *p1,*p2,*p3,*p4,*p5;				  //p2作为derxy
    NewPoint derold_x0y0={10000,10000};

    //每次都重置定位初始点，初始点选为中央
    x0y0=Base_station[4];

    for(n=0;n<10;++n)
    {
        for(i=0;i<N;++i)
        {
            fx0y0[i]=distance_sqr(&Base_station[num_sta[i]],&x0y0);
        }

        //矩阵 A 赋值   先X偏导部分后Y偏导
        for(int i=1,j=5;i!=N;++i,j=j+2)
        {
            *(L_juzhen+(i-1)) = ((int)vec[j]-(int)vec[3])*CHANGGE_M-fx0y0[i]+fx0y0[0]; //厘米为单位				     //精度改这里
            *(A_juzhen + 2*(i-1))=(x0y0.x-Base_station[num_sta[i]].x)/fx0y0[i]-(x0y0.x-Base_station[num_sta[0]].x)/fx0y0[0];
            *(A_juzhen + 2*(i-1)+1) =(x0y0.y-Base_station[num_sta[i]].y)/fx0y0[i]-(x0y0.y-Base_station[num_sta[0]].y)/fx0y0[0];
        }

        p1 = MatT_S( A_juzhen, N-1, 2);
        p3 = MatMul_S( p1, 2, N-1, A_juzhen, N-1, 2);
        p4 = MatInv_S(p3 , 2, 2);
        p5 = MatMul_S( p4, 2, 2, p1, 2, N-1);	 //	   (HT*H)-1*HT
        p2 = MatMul_S( p5, 2, N-1, L_juzhen, N-1, 1);

        x0y0.x+= *p2;
        x0y0.y+= *(p2+1);

        if(abs(*p2)>abs(derold_x0y0.x)&&abs(*(p2+1))>abs(derold_x0y0.y))
        {
            if(!lasts_x_y_pos[ID].isEmpty())
            {
                x0y0=lasts_x_y_pos[ID].back();
                break;
            }
            else
            {
                x0y0=Base_station[4];
                break;
            }
        }
        derold_x0y0.x = *p2;
        derold_x0y0.y = *(p2+1);

        if(fabs(derold_x0y0.x)<0.01&&fabs(derold_x0y0.y)<0.01)
            break;
    }
    delete p1;delete p2;delete p3;
    delete p4;delete p5;
}

void DataCenter::Taylor_test(const QVector<unsigned int>& vec)
{
    /* vec  中依次为   标签ID[0]  序列[1]
        1号基站基站ID[2] 时间[3]  1号基站基站ID[4] 时间[5]
        3号基站基站ID[6] 时间[7]  4号基站基站ID[8] 时间[9]   标签时钟[10]*/

    auto ID=vec[0];

    int n,i;
    double fx0y0[N];      //点到4基站之间的距离

    double A_juzhen[2*N],L_juzhen[N];
    double *p1,*p2,*p3,*p4,*p5;				  //p2作为derxy
    NewPoint derold_x0y0={10000,10000};

    //每次都重置定位初始点，初始点选为中央
    x0y0=Base_station[4];

    for(n=0;n<10;++n)
    {
        for(i=0;i<N;++i)
            fx0y0[i]=distance_sqr(&Base_station[num_sta[i]],&x0y0);

        //矩阵 A 赋值   先X偏导部分后Y偏导
        for(int i=0,j=5;i!=N;++i,j=j+2)
        {
            *(L_juzhen+i) = (static_cast<int>(vec[i]) - static_cast<int>(vec[10]))*CHANGGE_M-fx0y0[i]; //厘米为单位				     //精度改这里
            *(A_juzhen + 2*i)=(x0y0.x-Base_station[num_sta[i]].x)/fx0y0[i];
            *(A_juzhen + 2*i+1) =(x0y0.y-Base_station[num_sta[i]].y)/fx0y0[i];
        }

        p1 = MatT_S( A_juzhen, N, 2);
        p3 = MatMul_S( p1, 2, N, A_juzhen, N, 2);
        p4 = MatInv_S( p3 , 2, 2);
        p5 = MatMul_S( p4, 2, 2, p1, 2, N);	 //	   (HT*H)-1*HT
        p2 = MatMul_S( p5, 2, N, L_juzhen, N, 1);

        x0y0.x+= *p2;
        x0y0.y+= *(p2+1);

        if(abs(*p2)>abs(derold_x0y0.x)&&abs(*(p2+1))>abs(derold_x0y0.y))
        {
            x0y0=lasts_x_y_pos[ID].back();
            break;
        }
        derold_x0y0.x = *p2;
        derold_x0y0.y = *(p2+1);

        if(fabs(derold_x0y0.x)<0.01&&fabs(derold_x0y0.y)<0.01)
            break;
    }
    delete p1;delete p2;delete p3;
    delete p4;delete p5;
}

void DataCenter::Taylor_new(const QVector<unsigned int>& vec)
{
    /* vec  中依次为   标签ID[0]  序列[1]
        1号基站基站ID[2] 时间[3]  1号基站基站ID[4] 时间[5]
        3号基站基站ID[6] 时间[7]  4号基站基站ID[8] 时间[9]   标签时钟[10]*/

    auto ID=vec[0];
    double fx0y0[N];       //点到4基站之间的距离
    NewPoint derold_x0y0={10000,10000};

    //每次都重置定位初始点，初始点选为中央
    x0y0=Base_station[4];
    for(int n=0;n<10;++n)
    {
        for(int i=0;i<N;++i)
        {
            fx0y0[i]=distance_sqr(&Base_station[num_sta[i]],&x0y0);
        }
        //矩阵 A 赋值   先X偏导部分后Y偏导
        Matrix<double,3,2> H_matrix;
        Matrix<double,2,3> HT_matrix;
        Matrix<double,3,1> L_vec;
        Vector2d result;
        for(int i=1,j=5;i<N;++i,j=j+2)
        {
            L_vec[i-1] = (int)(vec[j]-vec[3])*CHANGGE_M-fx0y0[i]+fx0y0[0]; //厘米为单位				     //精度改这里
            H_matrix(i-1,0)=(x0y0.x-Base_station[num_sta[i]].x)/fx0y0[i]-(x0y0.x-Base_station[num_sta[0]].x)/fx0y0[0];
            H_matrix(i-1,1)=(x0y0.y-Base_station[num_sta[i]].y)/fx0y0[i]-(x0y0.y-Base_station[num_sta[0]].y)/fx0y0[0];
        }

        HT_matrix=H_matrix.transpose();
        result= (HT_matrix*H_matrix).inverse()*HT_matrix*L_vec;

        x0y0.x+= result[0];
        x0y0.y+= result[1];

        //若出现不收敛 ，将上一次该标签定位结果作为这一次结果
        if(abs(result[0])>abs(derold_x0y0.x)&&abs(result[1])>abs(derold_x0y0.y))
        {
            x0y0 =lasts_x_y_pos[ID].back();;
            break;
        }

        derold_x0y0.x =  result[0];
        derold_x0y0.y =  result[1];

        //若收敛满足要求，退出循环
        if(fabs(derold_x0y0.x)<0.01&&fabs(derold_x0y0.y)<0.01)
            break;
    }
}

void DataCenter::kf_settings(unsigned int ID)
{
    //卡尔曼滤波	 根据前一刻值xy[ID]调整现在得到的值x0y0
    //若该标签的数据第一次来
    if(nums[ID].size()==1)
    {
//        KM_data.new_label(ID);
//        KM_data.set_x_first(x0y0.x,ID);    //越界错误
//        KM_data.set_x_second(x0y0.y,ID);
        kf_params.x[ID][0] =  x0y0.x;
        kf_params.x[ID][1] =  x0y0.y;
    }
    else
    {
        //若存在三次数据
        if(nums[ID].size() == 3)
        {
            /*先求斜率，做大滤波*/
            int SEQ_DEC_1 = nums[ID].at(2)-nums[ID].at(1);
            int SEQ_DEC_2 = nums[ID].at(1)-nums[ID].at(0);
            int SEQ_DEC_3 = nums[ID].at(2)-nums[ID].at(0);

            if(SEQ_DEC_1<0)
                SEQ_DEC_1 += 256;
            if(SEQ_DEC_2<0)
                SEQ_DEC_2 += 256;
            if(SEQ_DEC_3<0)
                SEQ_DEC_3 += 256;

            auto k_0 = (x0y0.x - lasts_x_y_pos[ID].back().x)/SEQ_DEC_1 ;
            auto k_1= (lasts_x_y_pos[ID].back().x - lasts_x_y_pos[ID].front().x)/SEQ_DEC_2 ;

            if(fabs(k_0-k_1)/SEQ_DEC_3 > 1)
                x0y0 =lasts_x_y_pos[ID].back();
            else
            {
                auto k_2 = (x0y0.y - lasts_x_y_pos[ID].back().y)/SEQ_DEC_1 ;
                auto k_3 = (lasts_x_y_pos[ID].back().y - lasts_x_y_pos[ID].front().y)/SEQ_DEC_2 ;
                if(fabs(k_2-k_3)/SEQ_DEC_3 > 1)
                    x0y0 =lasts_x_y_pos[ID].back();
                else
                {
                    kf_params.z[0] = x0y0.x;   //设置当前时刻的观测位置  2/1  2行1列
                    kf_params.z[1] = x0y0.y;
            //        KM_data.set_z_first(x0y0.x);
            //        KM_data.set_z_second(x0y0.y);

                    kf_update(ID);
                    //kf_update_new(ID);
                }
            }
        }
    }
    //若存在两个数据，先pop
    if(lasts_x_y_pos[ID].size()==2)
        lasts_x_y_pos[ID].pop_front();
    lasts_x_y_pos[ID].push_back(x0y0);
}

void DataCenter::kf_update(unsigned int ID)
{
    int i;
    //第一个方程   x_ = kf_params.A * kf_params.x + kf_params.B * kf_params.u;			  kf_params.B * kf_params.u  一直为0
    //坐标估计值，即前一时刻的坐标位置
    double *x_ = MatMul_S( kf_params.A, 4, 4, kf_params.x[ID], 4, 1);

    //第二个方程   P_ = kf_params.A * kf_params.P * kf_params.A' + kf_params.Q;
    //估计值的均方误差
    double *p1 = MatMul_S( kf_params.A, 4, 4, kf_params.P[ID], 4, 4);
    double *p2 = MatT_S(kf_params.A, 4, 4) ;
    double *p3 = MatMul_S( p1, 4, 4, p2, 4, 4);
    double *P_ = MatAdd_S( kf_params.Q,p3, 4, 4);

    //第三个方程	   kf_params.K = P_ * kf_params.H' * (kf_params.H * P_ * kf_params.H' + kf_params.R)^-1;
    //滤波增益方程
    double *p4 = MatT_S(kf_params.H, 2, 4) ;
    double *p5 = MatMul_S( P_, 4, 4, p4, 4, 2);

    double *p6 = MatMul_S(kf_params.H, 2, 4, P_, 4, 4);
    double *p7 = MatMul_S(p6, 2, 4, p4, 4, 2);
    double *p8 = MatAdd_S( p7,kf_params.R, 2, 2);
    double *p9 = MatInv_S( p8, 2, 2);
    double *p10 = MatMul_S(p5, 4, 2, p9, 2, 2);
    for(i=0;i<8;i++)
        kf_params.K[i] = *(p10+i);

    //第四个方程		kf_params.x = x_ + kf_params.K * (kf_params.z - kf_params.H * x_);
    //更新坐标点
    double *p11 = MatMul_S(kf_params.H, 2, 4, x_, 4, 1);
    double *p12 = MatSub_S(kf_params.z,p11, 2, 1);
    double *p13 = MatMul_S(kf_params.K, 4, 2, p12, 2, 1);
    double *p14 = MatAdd_S( x_,p13, 4, 1);

    //第五个方程  	    kf_params.P = P_ - kf_params.K * kf_params.H * P_;
    //更新均方误差矩阵
    double *p15 = MatMul_S(kf_params.K, 4, 2, kf_params.H, 2, 4);
    double *p16 = MatMul_S(p15, 4, 4, P_, 4, 4);
    double *p17 = MatSub_S(P_,p16, 4, 4 );


    for(i=0;i<4;i++)
        kf_params.x[ID][i] = *(p14+i);
    for(i=0;i<16;i++)
        kf_params.P[ID][i] = *(p17+i);
    x0y0.x = kf_params.x[ID][0];
    x0y0.y = kf_params.x[ID][1];

    delete x_;delete p1;delete p2;delete p3;delete p4;delete p5;delete p6;
    delete p7;delete p8;delete p9;delete p10;delete p11;delete p12;delete p13;
    delete p14;delete p15;delete p16;delete p17;
}

//void DataCenter::kf_update_new(unsigned int ID)
//{
//    //第一个方程   x_ = kf_params.A * kf_params.x + kf_params.B * kf_params.u;  kf_params.B * kf_params.u  一直为0
//    //坐标估计值，即前一时刻的坐标位置
//    auto x_=KM_data.M_A()*KM_data.M_X(ID);           //4*1

//    //第二个方程   P_ = kf_params.A * kf_params.P * kf_params.A' + kf_params.Q;
//    //估计值的均方误差
//    auto p_=KM_data.M_A()*KM_data.M_P(ID) * KM_data.M_A().transpose() + KM_data.M_Q();

//    //第三个方程   kf_params.K = P_ * kf_params.H' * (kf_params.H * P_ * kf_params.H' + kf_params.R)^-1;
//    //滤波增益方程
//    auto p1=KM_data.M_H().transpose();
//    auto p2=(KM_data.M_H() * p_ * p1 +  KM_data.M_R()).inverse();
//    KM_data.M_K()= p_* p1 * p2;

//    //第四个方程	kf_params.x = x_ + kf_params.K * (kf_params.z - kf_params.H * x_);
//    //更新坐标点
//    auto p3=x_ + KM_data.M_K()*(KM_data.M_Z()-KM_data.M_H()*x_);
//    KM_data.set_x(p3,ID);

//    //第五个方程  kf_params.P = P_ - kf_params.K * kf_params.H * P_;
//    //更新均方误差矩阵
//    auto p4=p_ - KM_data.M_K()*KM_data.M_H()*p_;
//    KM_data.set_p(p4,ID);

//    x0y0.x = KM_data.M_X(ID)[0];
//    x0y0.y = KM_data.M_X(ID)[1];
//}

// 两点之间距离
double DataCenter::distance_sqr(NewPoint const* a, NewPoint const* b)
{
    return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
}

//转换为16进制表示的大写字符串
QString DataCenter::To_Upper(quint8 data)
{
    QString str;
    if(data<16)
        str="0"+QString::number(data,16).toUpper();
    else
        str=QString::number(data,16).toUpper();
    return str;
}

//向基站发送指令
void DataCenter::send_to_station(quint16 port_1,QByteArray ch)
{
    if( udpSocket->isValid())
        udpSocket->writeDatagram(ch, peerAddr,port_1);
}

