#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include "dialog_setting_1.h"
#include "ui_dialog_setting_1.h"

QString update_sql ="UPDATE settings SET X_1 =4.9 where rowid= 1";

Dialog_setting_1::Dialog_setting_1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_setting_1)
{
    ui->setupUi(this);

    QString aFile="soccer.db3";
    DB=QSqlDatabase::addDatabase("QSQLITE"); //添加 SQL LITE数据库驱动
    DB.setDatabaseName(aFile); //设置数据库名称

    if (!DB.open())   //打开数据库
    {
        QMessageBox::warning(this, "错误", "打开数据库失败",
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }
    openTable();
    //显示数据库内容
    show_seetings();
}

Dialog_setting_1::~Dialog_setting_1()
{
    delete ui;
}

//读取数据库内容
void Dialog_setting_1::openTable()
{   
    tabModel=new QSqlTableModel(this,DB);//数据表
    QString str="stations";
    tabModel->setTable(str); //设置数据表
    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);//数据保存方式，OnManualSubmit , OnRowChange
    tabModel->setSort(tabModel->fieldIndex("UUID"),Qt::AscendingOrder); //排序
    if (!(tabModel->select()))//查询数据
    {
       QMessageBox::critical(this, "错误信息",
              "打开数据表错误,错误信息\n"+tabModel->lastError().text(),
                 QMessageBox::Ok,QMessageBox::NoButton);
       return;
    }

    tabModel->setHeaderData(tabModel->fieldIndex("port"),Qt::Horizontal,"端口号");
    tabModel->setHeaderData(tabModel->fieldIndex("P_S"),Qt::Horizontal,"主从信息");
    tabModel->setHeaderData(tabModel->fieldIndex("UUID"),Qt::Horizontal,"UUID");
    tabModel->setHeaderData(tabModel->fieldIndex("IP"),Qt::Horizontal,"IP号");
    tabModel->setHeaderData(tabModel->fieldIndex("target"),Qt::Horizontal,"目标地址");
    tabModel->setHeaderData(tabModel->fieldIndex("X"),Qt::Horizontal,"X坐标(m)");
    tabModel->setHeaderData(tabModel->fieldIndex("Y"),Qt::Horizontal,"Y坐标(m)");
    tabModel->setHeaderData(tabModel->fieldIndex("distanceAB"),Qt::Horizontal,"距主基站距离(m)");

//    QDoubleSpinBox dou_box;
//    dou_box.setMaximum(1000);
//    dou_box.setMinimum(0);
//    dou_box.setDecimals(3);
//    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("X"),dou_box);
//    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Y"),dou_box);


    ui->tableView->setModel(tabModel);//设置数据模型
    theSelection=new QItemSelectionModel(tabModel);//关联选择模型
    ui->tableView->setSelectionModel(theSelection); //设置选择模型

    ui->tableView->setColumnHidden(tabModel->fieldIndex("distanceAB"),true);//隐藏列
    ui->tableView->verticalHeader()->setVisible(true); //隐藏列表头
    ui->tableView->horizontalHeader()->setVisible(true); //隐藏行表头
    ui->tableView->resizeColumnsToContents();//自动调整列宽
    ui->tableView->setAlternatingRowColors(true);


        //读取 设置表  并显示
    QSqlQuery query(DB);

    QString str_1,str_2,str_3,str_4;
    double dou_1,dou_2,dou_3,dou_4,dou_5;
    int num_1,num_2;

    //读取 version  并显示
    query.exec("select * from version");
    query.next();
    str_1=query.value("availableVersion").toString();
    str_2=query.value("currentVersion").toString();
    API_version ver(str_1,str_2);

    //读取 positioningArea  并显示
    query.exec("select * from positioningArea");
    query.next();
    str_1=query.value(0).toString();
    str_2=query.value(1).toString();
    str_3=query.value(2).toString();
    str_4=query.value(3).toString();
    API_positioningArea positi(str_1,str_2,str_3,str_4);

    //读取 ocs
    query.exec("select * from ocs");
    query.next();
    str_1=query.value(0).toString();
    str_2=query.value(1).toString();
    API_ocs ocs(str_1,str_2);

    //读取 coordinateReference
    query.exec("select * from coordinateReference");
    query.next();
    dou_1=query.value(0).toDouble();
    dou_2=query.value(1).toDouble();
    dou_3=query.value(2).toDouble();
    dou_4=query.value(3).toDouble();
    dou_5=query.value(4).toDouble();
    API_coordinateReference coord(ocs,dou_1,dou_2,dou_3,dou_4,dou_5);

    //读取 areaQualityInfo
    query.exec("select * from areaQualityInfo");
    query.next();
    dou_1=query.value(0).toDouble();
    dou_2=query.value(1).toDouble();
    str_1=query.value(2).toString();
    API_areaQualityInfo area(dou_1,dou_2,str_1);

    //读取 locationQualityInfo
    query.exec("select * from locationQualityInfo");
    query.next();
    num_1=query.value(0).toInt();
    dou_1=query.value(1).toDouble();
    dou_2=query.value(2).toDouble();
    str_1=query.value(3).toString();
    API_locationQualityInfo indoor_qua(num_1,dou_1,dou_2,str_1);
    query.next();
    num_1=query.value(0).toInt();
    dou_1=query.value(1).toDouble();
    dou_2=query.value(2).toDouble();
    str_1=query.value(3).toString();
    API_locationQualityInfo outdoor_qua(num_1,dou_1,dou_2,str_1);

    //读取 updateInterval
    query.exec("select * from updateInterval");
    query.next();
    num_1=query.value(0).toInt();
    num_2=query.value(1).toInt();
    str_1=query.value(2).toString();
    API_updateInterval update(num_1,num_2);

    API_positioningOption positioning(update,str_1,positi,coord,area);
    dou_1=0.0;
    dou_2=0.0;
    str_1="20180910T120530";
    API_indoorLocationData indoor(dou_1,dou_2,str_1,coord,indoor_qua);
    dou_1=coord.show_x();
    dou_2=coord.show_y();
    API_outdoorLocationData outdoor(dou_1,dou_2,str_1,coord,outdoor_qua);

    udp_json_API udp_saved(ver,positioning,outdoor,indoor);
    my_api=udp_saved;

}

//界面信息设置
void Dialog_setting_1::show_seetings()
{
    //室内外坐标联系
    ui->lineEdit_cos->setText(my_api.show_ocs());
    ui->SpinBox_X->setValue(my_api.show_coor_x());
    ui->SpinBox_Y->setValue(my_api.show_coor_y());
    ui->SpinBox_Z->setValue(my_api.show_coor_angle_x());
    ui->SpinBox_angle_x->setValue(my_api.show_coor_angle_x());
    ui->SpinBox_angle_z->setValue(my_api.show_coor_angle_z());

    //定位区域信息
    ui->lineEdit_num->setText(my_api.show_areaId());
    ui->lineEdit_name->setText(my_api.show_areaName());
    ui->lineEdit_addres->setText(my_api.show_areaAddress());
    ui->lineEdit_descrip->setText(my_api.show_areaDescription());

    //定位区域的平均质量信息
    ui->spinBox_H->setValue(my_api.show_areaH());
    ui->spinBox_V->setValue(my_api.show_areaV());

    //定位时间间隔（ms）
    ui->spinBox_default->setValue(my_api.show_defaultUpdateTime());
    ui->spinBox_min->setValue(my_api.show_minUpdateTime());

}

//界面信息设置
void Dialog_setting_1::updata_seetings_to_db3()
{
    QString str_1,str_2,str_3,str_4;
    double dou_1,dou_2,dou_3,dou_4,dou_5;

    //室内外坐标联系
    str_1=ui->lineEdit_cos->text();
    dou_1=ui->SpinBox_X->value();
    dou_2=ui->SpinBox_Y->value();
    dou_3=ui->SpinBox_Z->value();
    dou_4=ui->SpinBox_angle_x->value();
    dou_5=ui->SpinBox_angle_z->value();
    my_api.set_coordinateReference(str_1,dou_1,dou_2,dou_3,dou_4,dou_5);
    QSqlQuery query(DB);
    query.prepare("update coordinateReference set X=?, Y=?, Z=?,angle_x=?,angle_z=?");
    query.bindValue(0,dou_1);
    query.bindValue(1,dou_2);
    query.bindValue(2,dou_3);
    query.bindValue(3,dou_4);
    query.bindValue(4,dou_5);
    bool res=query.exec();
    if (!res)
        QMessageBox::information(this, "消息", "数据保存错误,错误信息\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);

    //定位区域信息
    str_1=ui->lineEdit_num->text();
    str_2=ui->lineEdit_name->text();
    str_3=ui->lineEdit_addres->text();
    str_4=ui->lineEdit_descrip->text();
    my_api.set_positioningArea(str_1,str_2,str_3,str_4);
    query.prepare("update positioningArea set areaId=?, areaName=?, areaAddress=?,areaDescription=?");
    query.bindValue(0,str_1);
    query.bindValue(1,str_2);
    query.bindValue(2,str_3);
    query.bindValue(3,str_4);
    res=query.exec();
    if (!res)
        QMessageBox::information(this, "消息", "数据保存错误,错误信息\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);

    //定位区域的平均质量信息
    dou_1=ui->spinBox_H->value();
    dou_2=ui->spinBox_V->value();
    my_api.set_areaQualityInfo(dou_1,dou_2);
    query.prepare("update areaQualityInfo set areaHorizAccuracy=?, areaVertAccuracy=?");
    query.bindValue(0,dou_1);
    query.bindValue(1,dou_2);
    res=query.exec();
    if (!res)
        QMessageBox::information(this, "消息", "数据保存错误,错误信息\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);

    //定位时间间隔（ms）
    dou_1=ui->spinBox_default->value();
    dou_2=ui->spinBox_min->value();
    my_api.set_updateInterval(dou_1,dou_2);
    query.prepare("update updateInterval set minUpdateTime=?, defaultUpdateTime=?");
    query.bindValue(0,dou_1);
    query.bindValue(1,dou_2);
    res=query.exec();
    if (!res)
        QMessageBox::information(this, "消息", "数据保存错误,错误信息\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);

    ui->lineEdit_cos->setEnabled(false);
    ui->SpinBox_X->setEnabled(false);
    ui->SpinBox_Y->setEnabled(false);
    ui->SpinBox_Z->setEnabled(false);
    ui->SpinBox_angle_x->setEnabled(false);
    ui->SpinBox_angle_z->setEnabled(false);

    ui->lineEdit_num->setEnabled(false);
    ui->lineEdit_name->setEnabled(false);
    ui->lineEdit_addres->setEnabled(false);
    ui->lineEdit_descrip->setEnabled(false);

    ui->spinBox_H->setEnabled(false);
    ui->spinBox_V->setEnabled(false);
    ui->spinBox_default->setEnabled(false);
    ui->spinBox_min->setEnabled(false);

}

//绑定、解绑与基站通信UDP端口
void Dialog_setting_1::on_pushButton_7_clicked()
{
    if(settings_true)
    {
        if(ui->pushButton_7->text()==tr("绑定端口"))
        {
            quint16 port=ui->udp_plot->text().toInt();
            emit send_udp_massage(port);
        }
        else
        {

            ui->udp_plot->setEnabled(true);       //恢复设置使能
            ui->pushButton_7->setText(tr("绑定端口"));
            emit send_udp_massage_close();
        }
    }
    else
        QMessageBox::warning(this,tr("警告"),tr("清先确认基础信息！！！"),QMessageBox::Ok);
}
//端口绑定，更改UI信息
void Dialog_setting_1::open_udp_true(bool open_udp)
{
    if(open_udp)
    {
        ui->udp_plot->setEnabled(false);
        ui->pushButton_7->setText(tr("解除绑定"));
    }
    else
        QMessageBox::warning(this,tr("警告"),tr("udp未正常绑定！！！"),QMessageBox::Ok);
}


//绑定、解绑与第三方API通信UDP端口
void Dialog_setting_1::on_pushButton_9_clicked()
{
    if(settings_true)
    {
        if(ui->pushButton_9->text()==tr("绑定端口"))
        {
            quint16 port_api=ui->udp_plot_api->text().toInt();
            emit open_api_udp(port_api);
        }
        else
        {
            ui->udp_plot_api->setEnabled(true);
            ui->pushButton_9->setText(tr("绑定端口"));
            emit send_udp_API_close();
        }
    }
    else
        QMessageBox::warning(this,tr("警告"),tr("清先确认基础信息！！！"),QMessageBox::Ok);
}
//端口绑定，更改UI信息
void Dialog_setting_1::open_API_udp_true(bool open_udp)
{
    if(open_udp)
    {
        ui->udp_plot_api->setEnabled(false);
        ui->pushButton_9->setText(tr("解除绑定"));
    }
    else
        QMessageBox::warning(this,tr("警告"),tr("udp未正常绑定！！！"),QMessageBox::Ok);
}

//确认基础信息无误
void Dialog_setting_1::on_pushButton_6_clicked()
{
    QMap<QString,int> my_map;
    for(int i = 0; i < 4; i++)
    {
        QAbstractItemModel *model = ui->tableView->model();
        QModelIndex index = model->index(i,1);
        QString data = model->data(index).toString();
        my_map[data]++;
    }
    if(my_map["主基站"]!=1)
    {
        QMessageBox::information(this, "消息", "主从信息错误！！！",
                                 QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        //更新基础信息
        updata_seetings_to_db3();
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_8->setEnabled(false);

        settings_true=true;

        my_api.clear_station();
        ui->tableView->setEnabled(false);
        //更新my_api基站信息
        QSqlQuery query(DB);
        query.exec("select * from stations");
        while(query.next())
        {
            int port=query.value(0).toInt();
            QString UUID=query.value(1).toString();
            QString P_S=query.value(2).toString();
            QString IP=query.value(3).toString();
            QString TARGET=query.value(4).toString();
            float x=query.value(5).toFloat();
            float y=query.value(6).toFloat();
            float dis=query.value(7).toFloat();

            API_stayion sta(port,UUID,P_S,IP,TARGET,x,y,dis);
            my_api.add_station(sta);
        }
        //依据主从信息调整主从距离
        my_api.reset_ps();

        emit send_my_api(my_api);

    }

}

//重置各项位置
void Dialog_setting_1::on_pushButton_5_clicked()
{
    if(ui->pushButton_7->text()=="绑定端口"&&ui->pushButton_9->text()=="绑定端口")
    {
        ui->lineEdit_cos->setEnabled(true);
        ui->SpinBox_X->setEnabled(true);
        ui->SpinBox_Y->setEnabled(true);
        ui->SpinBox_Z->setEnabled(true);
        ui->SpinBox_angle_x->setEnabled(true);
        ui->SpinBox_angle_z->setEnabled(true);

        ui->lineEdit_num->setEnabled(true);
        ui->lineEdit_name->setEnabled(true);
        ui->lineEdit_addres->setEnabled(true);
        ui->lineEdit_descrip->setEnabled(true);

        ui->spinBox_H->setEnabled(true);
        ui->spinBox_V->setEnabled(true);
        ui->spinBox_default->setEnabled(true);
        ui->spinBox_min->setEnabled(true);

        ui->tableView->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->pushButton_8->setEnabled(true);

        settings_true=false;
    }
    else
    {
        QMessageBox::warning(this,tr("警告"),tr("请先解除端口绑定！！！"),QMessageBox::Ok);
    }

}

void Dialog_setting_1::on_pushButton_3_clicked()
{
    if(first_open)
    {
        first_open=false;
        emit fir_open();
    }
}

//添加新基站
void Dialog_setting_1::on_pushButton_clicked()
{
    tabModel->insertRow(tabModel->rowCount(),QModelIndex()); //在末尾添加一个记录
    QModelIndex curIndex=tabModel->index(tabModel->rowCount()-1,1);//创建最后一行的ModelIndex
    theSelection->clearSelection();//清空选择项
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);//设置刚插入的行为当前选择行
}
//删除基站
void Dialog_setting_1::on_pushButton_2_clicked()
{
    QModelIndex curIndex=theSelection->currentIndex();//获取当前选择单元格的模型索引
    tabModel->removeRow(curIndex.row()); //删除
}
//保存基站信息
void Dialog_setting_1::on_pushButton_4_clicked()
{
    QMap<QString,int> my_map;
    for(int i = 0; i < 4; i++)
    {
        QAbstractItemModel *model = ui->tableView->model();
        QModelIndex index = model->index(1,i);
        QString data = model->data(index).toString();
        my_map[data]++;
    }
    if(my_map["主基站"]!=1)
    {
        QMessageBox::information(this, "消息", "主从信息错误！！！",
                                 QMessageBox::Ok,QMessageBox::NoButton);
    }
    else
    {
        bool res=tabModel->submitAll();

        if (!res)
            QMessageBox::information(this, "消息", "数据保存错误,错误信息\n"+tabModel->lastError().text(),
                                     QMessageBox::Ok,QMessageBox::NoButton);
    }


}
//取消保存
void Dialog_setting_1::on_pushButton_8_clicked()
{
     tabModel->revertAll();
}

