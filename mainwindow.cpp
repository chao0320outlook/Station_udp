#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chart.h"
#include "chartview.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QVector>
#include <QtGui/QImage>
#include <QtGui/QPainter>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //set_speed_time();
    //set_default_speed();
    dataCenter=new DataCenter(this);         //实例化数据处理中心
    my_setting=new Dialog_setting_1(this);   //实例化设置
    my_help=new Dialog_setting_2(this);      //实例化关于

    udp_API=new QUdpSocket(this);   //第三方接口初始化

    ui->chartView->chart()->legend()->setVisible(true);
    ui->doubleSpinBox->setEnabled(false);
    createChart_dot();

    /**************设置信息与主窗口的连接*************/
    //udp信号连接
    connect(my_setting,SIGNAL(send_udp_massage(quint16)),this,SLOT(accept_udp_massage(quint16)));
    connect(my_setting,SIGNAL(open_api_udp(quint16)),this,SLOT(udp_api_bind(quint16)));

    connect(this,SIGNAL(send_udp_to_setting(bool)),my_setting,SLOT(open_udp_true(bool)));
    connect(this,SIGNAL(send_API_udp_to_setting(bool)),my_setting,SLOT(open_API_udp_true(bool)));

    connect(my_setting,SIGNAL(send_udp_massage_close()),this,SLOT(close_udp_polt()));
    connect(my_setting,SIGNAL(send_udp_API_close()),this,SLOT(close_API_udp_polt()));

    //基站位置信息信号与datacenter连接
    connect(my_setting,SIGNAL(send_my_api(udp_json_API)),this,SLOT(json_receive(udp_json_API)));

    //模拟数据与主程序绘制信息连接
    connect(this,SIGNAL(random_data(unsigned int,double,double,QByteArray)),
            this,SLOT(updateChart(unsigned int,double,double,QByteArray)));

    my_setting->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dataCenter;
    delete my_setting;
    delete my_help;

    if (my_thread.isRunning())
    {
        my_thread.stopThread();
        my_thread.quit();
    }
    close_API_udp_polt();
    udp_API->close();
}

//窗口关闭事件，必须结束线程
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (my_thread.isRunning())
    {
        my_thread.stopThread();
        my_thread.quit();
    }
    udp_API->close();
    event->accept();

}


/**************        与第三方API交互         *****************/
//数据库内容同步至主函数
void MainWindow::json_receive(udp_json_API m_json)
{
    stations_deady=false;
    ui->comboBox->clear();

    my_json=m_json;
    QVector<float> sta=my_json.show_stations();

    //目前先这样
    dataCenter->set_Base_station_1_x(sta[0]);
    dataCenter->set_Base_station_1_y(sta[1]);
    dataCenter->set_Base_station_2_x(sta[2]);
    dataCenter->set_Base_station_2_y(sta[3]);
    dataCenter->set_Base_station_3_x(sta[4]);
    dataCenter->set_Base_station_3_y(sta[5]);
    dataCenter->set_Base_station_4_x(sta[6]);
    dataCenter->set_Base_station_4_y(sta[7]);
    //设置中心点
    dataCenter->set_center_point();

    //界面添加基站ID
    auto str_station=my_json.show_UUIDs();
    for(int i=0;i!=str_station.size();++i)
    {
         ui->comboBox->addItem(str_station[i],i);
    }
    stations_deady=true;
    on_comboBox_currentTextChanged(ui->comboBox->currentText());
}
//第三方接口绑定
void MainWindow::udp_api_bind(quint16 port)
{
    if(udp_API->bind(port))
    {
        emit send_API_udp_to_setting(true);
        QString udp_IP="";                //存储接收的IP地址，用于验证
        QString udp_IP_2="::ffff:"+udp_IP;
        connect(this->udp_API,SIGNAL(readyRead()),this,SLOT(json_receive_and_send()));  //udp接收与JSON解析绑定
        connect(&my_thread,SIGNAL(send_health_data()),this,SLOT(heart_data()));

        //发送心跳数据
        my_thread.start();
    }
    else
        emit send_API_udp_to_setting(false);
}
void MainWindow::close_API_udp_polt()
{
    my_thread.stopThread();       //关闭计时线程

    location_ready=false;
    start_send_to_API=false;

    while(udp_API->isValid())     //加循环是因为若正在传递数据，则不能正常关闭
    {
        udp_API->abort();
        udp_API->close();
    }
}

//向第三方发送心跳数据
void MainWindow::heart_data()
{
    if(!start_send_to_API)
    {
        QJsonObject result;
        result.insert("REPORT",QJsonValue("clientHeartBeat"));
        //result.insert("INVALID_PARAMETER",1010);
        //转化为json文件并发送
        QJsonDocument document;
        document.setObject(result);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);

        QString udp_IP="114.116.23.249";
        QHostAddress targetAddr(udp_IP);
        quint16 port=9003;
        udp_API->writeDatagram(byteArray,targetAddr,port);
    }
}
//第三方数据响应
void MainWindow::json_receive_and_send()
{
    while(udp_API->hasPendingDatagrams())
    {
        QByteArray   datagram;
        datagram.resize(udp_API->pendingDatagramSize());
        udp_API->readDatagram(datagram.data(),datagram.size(),&API_Addr,&API_Port);

        QJsonObject result;
        int num=json_analysis(datagram);       //对请求进行分析辨认
        if(num<5)
        {
            if(simulation)         //未绑定基站udp
                result.insert("NO_CONNECTION_TO_POSITIONING",1009);
            else if(new_data)           //一切正常，对命令做出响应
            {
                switch(num)
                {                       //响应元数据
                case 1:
                    result=my_json.show_1_Capabilitie();
                    break;

                case 2:                  //响应初始化
                    location_ready=true;
                    result=my_json.show_2_Initialize();
                    break;

                case 3:                      //开启定位服务
                    if(location_ready)
                    {
                        start_send_to_API=true;
                        //result=my_json.show_3_Start();          //非测试阶段注释掉！！！
                    }
                    else
                        result.insert("NO_CONNECTION_TO_POSITIONING",1009);
                    break;

                case 4:                  //关闭定位服务
                    if(start_send_to_API)
                    {
                        start_send_to_API=false;
                        location_ready=false;
                        result=my_json.show_4_Stop();
                    }
                    else
                        result.insert("INVALID_PARAMETER",1010);
                    break;
                }
            }
            else           //基站无数据传入
            {
                 result.insert("SENSOR_ERROR",1008);
            }

        }

        else                 //指令有误
        {
            switch(num)
            {
            case 5:
                result.insert("IP_BAD_REQUEST",1001);
                break;
            case 6:
                result.insert("INVALID_PARAMETER",1010);
                break;
            case 7:
                result.insert("MISSING_PARAMETER",1011);
                break;
            }
        }

        if(!result.empty())
        {
            //转化为json文件并发送
            QJsonDocument document;
            document.setObject(result);
            QByteArray byteArray = document.toJson(QJsonDocument::Compact);

            udp_API->writeDatagram(byteArray,API_Addr,API_Port);
        }
    }
}
//对请求进行分析辨认
int MainWindow::json_analysis(QByteArray& data)
{
    //  无法识别——5，  参数无效——6   缺少参数——7  参数多余——8
    int result=0;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(data, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) // 解析未发生错误
    {
        if (doucment.isObject())  // JSON 文档为对象
        {
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("REQUEST"))              // 包含指定的 key
            {
                QJsonValue value = object.value("REQUEST");  // 获取指定 key 对应的 value

                result=6;
                if (value.isString()) // 判断 value 是否为字符串
                {
                    QString  request_now= value.toString();  // 将 value 转化为字符串

                    if(request_now.isEmpty())
                    {
                        result=7;         //缺少参数
                    }
                    if(request_now==request[2])
                        result=3;
                    else if(request_now==request[0])
                        result=1;
                    else if(request_now==request[1])
                        result=2;
                    else if(request_now==request[3])
                        result=4;
                }
            }
            else
            {
                result=5;
            }


            if(object.contains("CLIENT"))
            {
                if (result==3||result==4)
                {
                    QJsonValue value = object.value("CLIENT");
                    if (value.isString())
                    {
                        QString client_now = value.toString();  // 将 value 转化为字符串
                        if(client_now.isEmpty())
                        {
                            result=7;         //缺少参数
                        }
                        else if(client_now!=client[0])
                            result=6;
                    }
                    else
                    {
                        result=6;
                    }
                }
                else
                    result=5;
            }
            else  //缺少第二个键值对
            {
                if (result==3||result==4)
                    result=7;
            }
        }
        else
        {
            result=5;
        }
    }
    else
    {
        result=5;
    }

    return result;
}




/**************       与基站绑定并发送指令       *****************/
//udp绑定基站端口 用于数据接收
void MainWindow::accept_udp_massage(quint16 port)
{
    bool flag=dataCenter->bind_udp(port);
    emit send_udp_to_setting(flag);

    //更改绘制模式
    if(flag)
    {
        simulation=false;
        ui->label_2->setText("当前模式：标签定位");
        ui->pushButton->setText("暂停");

        //dataCenter 中 处理后数据 与主程序连接
        connect(dataCenter,SIGNAL(send_new_data(unsigned int,double,double,QByteArray)),
                this,SLOT(updateChart(unsigned int,double,double,QByteArray)));

        connect(dataCenter,SIGNAL(new_data_test(QString)),this,SLOT(show_data_1(QString )));
        connect(dataCenter,SIGNAL(new_wrong_data_test(QString)),this,SLOT(show_data_2(QString )));
        connect(dataCenter,SIGNAL(new_on_line_station(QString)),this,SLOT(show_data_online_station(QString )));

        connect(dataCenter,SIGNAL(new_feedback(bool,quint16)),this,SLOT(show_feedback(bool ,quint16)));
        connect(dataCenter,SIGNAL(new_message(QByteArray,quint16)),this,SLOT(analysis_message(QByteArray,quint16)));
    }
}
void MainWindow::close_udp_polt()
{
    //解绑端口
    dataCenter->close_udp();
    simulation=true;   //更改为模拟模式
    stop_accept=false;  //允许接收数据

    ui->label_2->setText("当前模式：模拟模式");
    ui->pushButton->setText("比赛开始");

    //模拟数据与主程序绘制信息连接
    connect(this,SIGNAL(random_data(unsigned int,double,double,QByteArray)),
            this,SLOT(updateChart(unsigned int,double,double,QByteArray)));

}

//发送获取信息
void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    if(stations_deady)
    {
        quint16 port_1=my_json.show_station_port(arg1);
        char ch_1[]={0x01,0x20,0x05,'\r','\n'};
        QByteArray ch(ch_1,5);
        dataCenter->send_to_station(port_1,ch);

        ui->sta_x->setValue(my_json.show_station_X(arg1));
        ui->sta_y->setValue(my_json.show_station_Y(arg1));
    }

}
//发送获取信息
void MainWindow::on_pushButton_9_clicked()
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    char ch_1[]={0x01,0x20,0x05,'\r','\n'};
    QByteArray ch(ch_1,5);
    dataCenter->send_to_station(port_1,ch);
}
//重启
void MainWindow::on_pushButton_11_clicked()
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    char ch_1[]={0x01,0x20,0x01,'\r','\n'};
    QByteArray ch(ch_1,5);
    dataCenter->send_to_station(port_1,ch);
}
//自检
void MainWindow::on_pushButton_12_clicked()
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    char ch_1[]={0x01,0x20,0x02,'\r','\n'};
    QByteArray ch(ch_1,5);
    dataCenter->send_to_station(port_1,ch);
}
//定位
void MainWindow::on_pushButton_13_clicked()
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    char ch_1[]={0x01,0x20,0x03,'\r','\n'};
    QByteArray ch(ch_1,5);
    dataCenter->send_to_station(port_1,ch);
}
//测距
void MainWindow::on_pushButton_14_clicked()
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    char ch_1[]={0x01,0x20,0x04,'\r','\n'};
    QByteArray ch(ch_1,5);
    dataCenter->send_to_station(port_1,ch);
}
//发送信息
void MainWindow::on_pushButton_10_clicked()
{
    QString str=ui->lineEdit_2->text();
    if(str.size()!=6)
    {
       QMessageBox::warning(this,tr("错误"),tr("请输入6位数字（字母）的目标ID！！！"),QMessageBox::Ok);
    }
    else
    {
        QString index=ui->comboBox->currentText();
        quint16 port_1=my_json.show_station_port(index);
        char ch_1[]={0x01,0x20,0x06};

        //设置基站模式
        QByteArray ch(ch_1,3);
        if(ui->comboBox_2->currentIndex()==0)
            ch.push_back(0x01);
        else
            ch.push_back(0x02);

        //添加7位无用信息
        for(int i=0;i!=7;++i)
            ch.push_back(0x30);

        //添加目标ID
        ch.append(str);
        float fla=ui->doubleSpinBox->value();

        //添加与主基站距离
        QByteArray dta;
        dta.resize(sizeof(fla));    //重新调整QByteArray的大小
        memcpy(dta.data(),&fla,sizeof(fla));
        ch.push_back(dta);

        // 添加/r /n
        ch.push_back(0x0d);
        ch.push_back(0x0a);

        dataCenter->send_to_station(port_1,ch);
    }
}

/**************        解析基站回馈信息         *****************/
//测试信息显示
void MainWindow::show_data_1(QString str)
{
    ui->textEdit->setText(str);
    if(save_data)
    {
        data_save.push_back(str);
    }
//    ui->textEdit->insertPlainText(str+" ");
//    ui->textEdit->moveCursor(QTextCursor::End);
//    ui->textEdit->setText(str);
}
//回馈信息处理并显示
void MainWindow::show_data_online_station(QString str)
{
    ui->label_3->setText("当前工作基站："+str);
}
void MainWindow::show_feedback(bool flag,quint16 port)
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    if(port==port_1)
    {
            if(flag)
                QMessageBox::warning(this, "信息", index+"号基站写入命令成功！！！",
                                         QMessageBox::Ok,QMessageBox::NoButton);
            else
                QMessageBox::warning(this, "错误", index+"号基站写入命令失败！！！",
                                         QMessageBox::Ok,QMessageBox::NoButton);
    }
    //更新数据库

}
void MainWindow::analysis_message(QByteArray message,quint16 port)
{
    QString index=ui->comboBox->currentText();
    quint16 port_1=my_json.show_station_port(index);
    //若为当前所选基站
    if(port==port_1)
    {
        if((quint8)message[0]==1)
            ui->comboBox_2->setCurrentIndex(0);
        else
            ui->comboBox_2->setCurrentIndex(1);

        switch (message[1])
        {
        case 0x01:
            ui->label_9->setText("当前行为模式：测距");
            break;
        case 0x02:
            ui->label_9->setText("当前行为模式：定位");
            break;
        case 0x03:
            ui->label_9->setText("当前行为模式：自检");
            break;
        default:
            break;
        }
        QString res;
        for(int i=8;i!=14;++i)
        {
            res.push_back((char)message[i]);
        }
        ui->lineEdit_2->setText(res);

        QByteArray arr;
        arr.resize(4);
        for(int i=14;i!=18;++i)
        {
            arr.push_back(message[i]);
        }
        float a;
        memcpy(&a,arr.data(),4);
        ui->doubleSpinBox->setValue(a);
    }
}



/**************        实时位置散点图构建与绘制        *****************/
//创建图表的各个部件_散点图
void MainWindow::createChart_dot()
{
    Chart *chart = new Chart();
    chart->setTitle(tr("实时散点定位图"));

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartView->chart()->legend()->setAlignment(Qt::AlignTop);  //图例位置

    QMargins    mgs;
    mgs.setLeft(5);
    mgs.setRight(5);
    mgs.setTop(5);
    mgs.setBottom(5);
    ui->chartView->chart()->setMargins(mgs);                  //设置图表的四个边距

    QValueAxis *axisX = new QValueAxis;

    axisX->setRange(0, 100); //设置坐标轴范围
    axisX->setTickCount(11); //主分隔个数

    axisX->setLabelFormat("%.1f"); //标签格式
    axisX->setMinorTickCount(6);
    axisX->setTitleText("X轴位移(m)"); //标题

    ui->chartView->chart()->setAxisX(axisX);

    QValueAxis *axisY = new QValueAxis;

    axisY->setRange(0,60); //设置坐标轴范围
    axisY->setTickCount(7); //主分隔个数

    axisY->setTitleText("value");
    axisY->setLabelFormat("%.2f"); //标签格式
    axisY->setMinorTickCount(6);
    axisY->setTitleText("Y轴位移(m)"); //标题

    ui->chartView->chart()->setAxisY(axisY);

    team1_dot.clear();

    on_cBoxTheme_currentIndexChanged(ui->cBoxTheme->currentIndex());
    on_cBoxAnimation_currentIndexChanged(ui->cBoxAnimation->currentIndex());
}
//获取实时位置信息并添加
void MainWindow::updateChart(unsigned int ID,double x,double y,QByteArray health_str)
{
    new_data=true;
    //向第三方API发送数据
    if(start_send_to_API)
    {
        QString str_1;
        QByteArray vec("00000");
        for(const auto &i:vec)
        {
            str_1+=QString::number(i,16).toUpper();
        }
        if(ID<16)
            str_1+="0"+QString::number(ID,16).toUpper();
        else
            str_1+= QString::number(ID,16).toUpper();

        QString str_2;
        for(const auto &i:health_str)
        {
            str_2+=QString::number(i,16).toUpper();
        }

        my_json.set_x_y(x*100,y*100,str_1,str_2);     //坐标记得修改！！！！
        QJsonObject result=my_json.show_3_Start();

        QJsonDocument document;
        document.setObject(result);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);

        udp_API->writeDatagram(byteArray,API_Addr,API_Port);
    }

    if(!stop_accept)
    {
        unsigned int &ID_now=ID;
        auto ptr_play=vec_label.find(ID_now);         //确认储存中是否有该对应信息

        //收到新的标签号
        if(ptr_play==vec_label.end())
        {
            team1_point_true.push_back(false);
            QString str3=QString::number(ID_now)+"号标签";

            ui->combox_mine->addItem(str3,i_count);

            QString str="(     )";
            QString str2="";
            vec_str_location.push_back(str);
            vec_temp.push_back(str2);
            vec_speed_2.push_back(str2);
            vec_speed_1.push_back(default_speed);

            vec_label[ID_now]=i_count;             //标签号与vec_line的对应
            i_count++;

            //在图表内添加该标签
            QScatterSeries *series0=new QScatterSeries();
            series0->setName(str3+"号 ");
            series0->setMarkerSize(11);
            series0->setPointsVisible(true);

            QChart *chart =ui->chartView->chart();
            chart->addSeries(series0);

            chart->setAxisX(chart->axisX(), series0); //添加X坐标轴
            chart->setAxisY(chart->axisY(), series0); //添加Y坐标轴
            team1_dot.push_back(series0);
        }

        if(Drawing_true)
        {
            auto zz=vec_label[ID_now];
            QScatterSeries *series0=team1_dot[zz];
            if(!team1_point_true[zz])
                series0->clear();
            *series0<<QPointF(x,y);  //添加新点

            //将实时数据点存储
            vec_str_location[zz]="("+QString::number(x, 10, 3)+","+QString::number(y, 10, 3)+")";
            if(zz==ui->comboBox->currentIndex())
                ui->label_5->setText(vec_str_location[zz]);
        }
    }
}

//绘制开始
void MainWindow::on_pushButton_6_clicked()
{
    if(ui->pushButton_6->text()=="开始绘制")
    {
        Drawing_true=true;
        ui->pushButton_6->setText("结束绘制");
    }
    else
    {
        Drawing_true=false;
        flag_1=false;
        ui->pushButton_6->setText("开始绘制");
        if(simulation)
            ui->pushButton->setText("比赛开始");
        else
        {
            ui->pushButton->setText("暂停");
        }
    }
}
//开始比赛，启用随机数
void MainWindow::on_pushButton_clicked()
{
    if(Drawing_true)     //开始绘制按钮按下
    {
        if(simulation)      //若为模拟模式
        {
            if(ui->pushButton->text()=="比赛开始")
            {
                ui->pushButton->setText("比赛结束");
                flag_1=true;

                random_data_create();
            }
            else
            {
                ui->pushButton->setText("比赛开始");
                flag_1=false;      //随机数停止生成
            }
        }
        else
        {
            if(ui->pushButton->text()=="暂停")
            {
                ui->pushButton->setText("继续");
                stop_accept=true;
            }
            else
            {
                ui->pushButton->setText("暂停");
                stop_accept=false;
            }
        }
    }
    else
    {
        QMessageBox::warning(this,tr("提示"),tr("请先按下 开始绘制 按钮！！！"),QMessageBox::Ok);
    }

}
//随机数据生成
void MainWindow::random_data_create()
{  
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QByteArray str("ABCDEFGHIGKLMNOPQRSTUVWXYZ1234567890");
    while(flag_1)
    {
        unsigned int ID=1;
        int a_1,a_2,a_3,b_1,b_2,b_3;
        for(int t=0;t!=5;t++)
        {
            a_1 = qrand()%10;
            a_2 = qrand()%10;
            a_3 = qrand()%10;

            b_1 = qrand()%10;
            b_2 = qrand()%10;
            b_3 = qrand()%10;

            auto x = qrand()%50+a_1*0.1+a_2*0.01+a_3*0.001;
            auto y = qrand()%60+b_1*0.1+b_2*0.01+b_3*0.001;

            emit random_data(ID,x,y,str);
            sleep(1000);
            ID++;
        }

//        ID=11;
//        for(int t=0;t!=5;t++)
//        {
//            a_1 = qrand()%10;
//            a_2 = qrand()%10;
//            a_3 = qrand()%10;

//            b_1 = qrand()%10;
//            b_2 = qrand()%10;
//            b_3 = qrand()%10;

//            auto x = 50+qrand()%50+a_1*0.1+a_2*0.01+a_3*0.01;
//            auto y = qrand()%60+b_1*0.1+b_2*0.01+b_3*0.01;

//            emit random_data(ID,x,y,str);
//            sleep(1000);
//            ID++;
//        }
    }
}
//延时程序
void MainWindow::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}



/**************        界面的一些细节设置        *****************/
//动画效果
void MainWindow::on_cBoxAnimation_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->chartView->chart()->setAnimationOptions(QChart::NoAnimation);
        break;
    case 1:
        ui->chartView->chart()->setAnimationOptions(QChart::GridAxisAnimations);
        break;
    case 2:
        ui->chartView->chart()->setAnimationOptions(QChart::SeriesAnimations);
        break;
    default:
        ui->chartView->chart()->setAnimationOptions(QChart::NoAnimation);
        break;
    }
}
//图表的主题
void MainWindow::on_cBoxTheme_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->chartView->chart()->setTheme(QChart::ChartThemeQt);
        break;
    case 1:
        ui->chartView->chart()->setTheme(QChart::ChartThemeBrownSand);
        break;
    case 2:
        ui->chartView->chart()->setTheme(QChart::ChartThemeBlueCerulean);
        break;
    case 3:
        ui->chartView->chart()->setTheme(QChart::ChartThemeDark);
        break;
    default:
        ui->chartView->chart()->setTheme(QChart::ChartThemeQt);
        break;
    }
}
//绘线是否开启
void MainWindow::on_checkBox_clicked(bool checked)
{
    auto index=ui->combox_mine->currentIndex();
    team1_point_true[index]=checked;
}
//全部绘制
void MainWindow::on_checkBox_5_clicked(bool checked)
{
    for(auto ptr=team1_point_true.begin();ptr!=team1_point_true.end();ptr++)
        *ptr=checked;
}
//UI界面标签选择变化
void MainWindow::on_combox_mine_currentIndexChanged(int index)
{
    if(team1_dot.size()>=index+1)          //用于避免越界错误
    {
        //createChart_speed_1(index);
        curSeries_dot=team1_dot[index_big_1];
        curSeries_dot->setMarkerSize(11);
        curSeries_dot=team1_dot[index];
        curSeries_dot->setMarkerSize(15);
        index_big_1=index;
        ui->label_5->setText(vec_str_location[index]);

        if(team1_point_true[index])
            ui->checkBox->setChecked(true);
        else
            ui->checkBox->setChecked(false);
    }

}
//设置显示
void MainWindow::on_action_set_triggered()
{
    my_setting->show();
}
//关于菜单
void MainWindow::on_action_help_triggered()
{
    my_help->show();
}


//存储数据
void MainWindow::on_checkBox_2_clicked(bool checked)
{
    save_data=checked;
    if(!save_data)
    {
        if(!data_save.empty())
        {
            QString fileName = QFileDialog::getSaveFileName
                    (this, tr("存储数据"), "",tr("文本(*.txt)"));
            if (fileName.isEmpty())
                return;
            else
            {
                QFile file(fileName);
                if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
                {
                    QMessageBox::information(this, tr("Unable to open file"),file.errorString());
                    return;
                }

                QString str_all="ID       X(m)       Y(m)        t1         t2           t3\n";
                for(auto ptr=data_save.begin();ptr!=data_save.end();ptr++)
                {
                    QString str= *ptr + "\n";
                    str_all.push_back(str);
                }
                QDataStream out(&file);
                out.setVersion(QDataStream::Qt_4_5);
                out << str_all;
            }
        }
        data_save.clear();
    }
}

//测试用
void MainWindow::on_pushButton_2_clicked()
{
    QJsonObject result;
    result=my_json.show_2_Initialize();
    //转化为json文件并发送
    QJsonDocument document;
    document.setObject(result);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString udp_IP="114.116.23.249";
    QHostAddress targetAddr(udp_IP);
    quint16 port=9003;
    udp_API->writeDatagram(byteArray,targetAddr,port);
}
void MainWindow::on_pushButton_8_clicked()
{
    QJsonObject result;
    result.insert("REPORT",QJsonValue("clientHeartBeat"));
    //result.insert("INVALID_PARAMETER",1010);
    //转化为json文件并发送
    QJsonDocument document;
    document.setObject(result);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString udp_IP="114.116.23.249";
    QHostAddress targetAddr(udp_IP);
    quint16 port=9003;
    udp_API->writeDatagram(byteArray,targetAddr,port);
}

QString MainWindow::To_Upper(quint8 data)
{
    QString str;
    if(data<16)
        str="0"+QString::number(data,16).toUpper();
    else
        str=QString::number(data,16).toUpper();
    return str;
}


/**************        加速度实时图表，暂时不用        *****************/
////初始化加速度表
//void MainWindow::set_speed_time()
//{
//    for(float i=0;i<10;i=i+0.2)
//    {
//        speed_time.push_back(i);
//    }
//}
//void MainWindow::set_default_speed()
//{
//    for(float i=0;i<70;++i)
//    {
//        default_speed.push_back(0.0);
//    }
//}
////创建加速度实时显示图
//void MainWindow::createChart_speed_1(int index)
//{
//    QChart *chart = new QChart();
//    chart->setTitle(tr("实时加速度图"));
//    ui->chartView_2->setChart(chart);
//    ui->chartView_2->setRenderHint(QPainter::Antialiasing);
//    ui->chartView_2->chart()->legend()->setVisible(false);

//    QMargins    mgs;
//    mgs.setLeft(1);
//    mgs.setRight(1);
//    mgs.setTop(1);
//    mgs.setBottom(1);
//    ui->chartView_2->chart()->setMargins(mgs);                  //设置图表的四个边距

//    //折线序列的线条设置
//    QPen    pen;
//    pen.setStyle(Qt::NoPen);
//    pen.setWidth(2);

//    QValueAxis *axisX = new QValueAxis;

//    axisX->setRange(0, 10); //设置坐标轴范围
//    axisX->setLabelFormat("%.1f"); //标签格式
//    axisX->setTickCount(6); //主分隔个数
//    axisX->setMinorTickCount(4);
//    axisX->setTitleText("t时间(s)"); //标题

//    ui->chartView_2->chart()->setAxisX(axisX);

//    QValueAxis *axisY = new QValueAxis;
//    axisY->setRange(-2, 4);
//    axisY->setTitleText("value");
//    axisY->setTickCount(7);
//    axisY->setLabelFormat("%.2f"); //标签格式

//    axisY->setMinorTickCount(4);
//    axisY->setTitleText("加速度(g)"); //标题

//     ui->chartView_2->chart()->setAxisY(axisY);

//    m_pen.setStyle(Qt::PenStyle(0)); //线型

//    QLineSeries *series0=new QLineSeries();
//    series0->setName(QString::number(vec_label[index],10)+"号标签");
//    series0->setPointsVisible(false);
//    chart->addSeries(series0);

//    chart->setAxisX(axisX, series0); //添加X坐标轴
//    chart->setAxisY(axisY,series0); //添加Y坐标轴

//    series_speed=series0;
//}

