#ifndef UDP_JSON_API_H
#define UDP_JSON_API_H

#include <QVector>
#include <math.h>
#include <QString>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

//基站类
class API_stayion
{
public:
    API_stayion()=default;
    API_stayion(int num,QString str_1,QString str_2,QString str_3,QString str_4,float flo_1,float flo_2,
                float dis):port(num), p_s(str_1),UUID(str_2),IP(str_3),target(str_4),
        x(flo_1),y(flo_2),distance(dis){}
    float X()const{return x;}
    float Y()const{return y;}
    float distanceAB()const{return distance;}
    QString show_UUID()const{return UUID;}
    int show_IP()const{return port;}
    int show_port()const {return port;}
    const QString& show_p_s()const{return p_s;}
    void set_distance(float x1,float y1)
    {
        distance=sqrt(pow(x1-x,2)+pow(y1-y,2));
    }
private:
    int port;
    QString p_s;
    QString UUID;
    QString IP;
    QString target;
    float x;
    float y;
    float distance;
};

//版本类
class API_version
{
public:
    API_version()=default;
    API_version(QString str_1,QString str_2);

    QString show_availableVersion()const{return availableVersion;}
    QString show_currentVersion()const{return currentVersion;}
    void set_availableVersion(QString str){availableVersion=str;}
    void set_currentVersion(QString str){currentVersion=str;}

    QJsonObject show()const{return version;}
private:
    QString availableVersion;        //有效版本
    QString currentVersion;          //当前版本
    QJsonObject version;
};

//定位区域信息 类
class API_positioningArea
{
public:
    API_positioningArea()=default;
    API_positioningArea(QString str_1,QString str_2,QString str_3,QString str_4);

    QString show_areaId()const{return areaId;}
    QString show_areaName()const{return areaName;}
    QString show_areaAddress()const{return areaAddress;}
    QString show_areaDescription()const{return areaDescription;}

    void set_areaId(QString str){areaId=str;}
    void set_areaName(QString str){areaName=str;}
    void set_areaAddress(QString str){areaAddress=str;}
    void set_areaDescription(QString str){areaDescription=str;}

    QJsonObject show()const{return positioningArea;}

private:
    QString areaId;              //定位区域编号
    QString areaName;            //定位区域名称
    QString areaAddress;         //定位区域地址
    QString areaDescription;     //定位区域描述信息

    QJsonObject positioningArea;
};

//室外坐标系采用坐标版本
class API_ocs
{
public:
    API_ocs()=default;
    API_ocs(QString str_1,QString str_2);

    QString show_name()const{return name;}
    QString show_description()const{return description;}
    void set_name(QString str){name=str;}
    void set_description(QString str){description=str;}

    QJsonObject show()const{return ocs;}

private:
    QString name;             //室外坐标系名称
    QString description;      //室外坐标系描述信息

    QJsonObject ocs;
};
//坐标参考系
class API_coordinateReference
{
public:
    API_coordinateReference()=default;
    API_coordinateReference(API_ocs ocs_1,double x_1,double y_1,double z_1,double an_x,double an_z);

    API_ocs show_ocs()const {return ocs;}
    QString show_ocs_name() const {return ocs.show_name();}
    double show_x()const{return x;}
    double show_y()const{return y;}
    double show_z()const{return z;}
    double show_angle_x()const{return angle_x;}
    double show_angle_z()const{return angle_z;}

    void set_ocs(API_ocs ocs_1){ocs=ocs_1;}
    void set_ocs_name(QString str){ocs.set_name(str);}
    void set_x(double dou){x=dou;}
    void set_y(double dou){y=dou;}
    void set_z(double dou){z=dou;}
    void set_angle_x(double dou){angle_x=dou;}
    void set_angle_z(double dou){angle_z=dou;}

    QJsonObject show()const{return coordinateReference;}
private:
    API_ocs ocs;
    double x,y,z;               //室内坐标系原点对应的室外 X Y Z 坐标
    double angle_x,angle_z;     //室内的 x 轴与真北方向顺时针的夹角  室内的 z 轴与地心垂直的顺时针的夹角

    QJsonObject coordinateReference;

};

//定位区域的平均质量信息
class API_areaQualityInfo
{
public:
    API_areaQualityInfo()=default;
    API_areaQualityInfo(double dou_1,double dou_2,QString str);

    QString show_unit()const{return unit;;}
    double show_areaHorizAccuracy()const{return areaHorizAccuracy;}
    double show_areaVertAccuracy()const{return areaVertAccuracy;}

    void set_unit(QString str){unit=str;}
    void set_areaHorizAccuracy(double dou){areaHorizAccuracy=dou;}
    void set_areaVertAccuracy(double dou){areaVertAccuracy=dou;}

    QJsonObject show()const{return areaQualityInfo;}
private:
    double areaHorizAccuracy;         //平均水平精度
    double areaVertAccuracy;          //平均垂直精度
    QString unit;                     //精度单位

    QJsonObject areaQualityInfo;
};

//定位质量
class API_locationQualityInfo
{
public:
    API_locationQualityInfo()=default;
    API_locationQualityInfo(int num,double dou_1,double dou_2,QString str);

    int show_qualityIndicator()const{return qualityIndicator;}
    double show_horizAccuracy()const{return horizAccuracy;}
    double show_vertAccuracy()const{return vertAccuracy;}
    QString show_unit()const{return unit;}

    void set_qualityIndicator(int num){qualityIndicator=num;}
    void set_horizAccuracy(double dou){horizAccuracy=dou;}
    void set_vertAccuracy(double dou){vertAccuracy=dou;}
    void set_unit(QString str){unit=str;}

    QJsonObject show()const{return locationQualityInfo;}
private:
    int qualityIndicator;     //定位质量指示，采用枚举（0-定位有效； 1-定位无效； 2-定位修正）
    double horizAccuracy;     //水平精度
    double vertAccuracy;      //垂直精度
    QString unit;             //精度单位

    QJsonObject locationQualityInfo;

};

//定位时间间隔
class API_updateInterval
{
public:
    API_updateInterval()=default;
    API_updateInterval(int num,int num_2);

    int show_minUpdateTime()const{return  minUpdateTime;}
    int show_defaultUpdateTime()const{return  defaultUpdateTime;}

    void set_minUpdateTime(int num){minUpdateTime=num;}
    void set_defaultUpdateTime(int num){defaultUpdateTime=num;}

    QJsonObject show()const{return updateInterval;}
private:
    int minUpdateTime;          //最小定位时间间隔，单位毫米
    int defaultUpdateTime;      //默认定位时间间隔，单位毫米

    QJsonObject updateInterval;
};

//元数据
class API_positioningOption
{
public:
    API_positioningOption()=default;
    API_positioningOption(API_updateInterval api_1, QString str,API_positioningArea api_2,
                          API_coordinateReference api_3,API_areaQualityInfo api_4);

    QJsonObject show()const{return positioningOption;}

    QString show_ocs_name() const {return coordinateReference.show_ocs_name();}
    double show_x() const {return coordinateReference.show_x();}
    double show_y() const {return coordinateReference.show_y();}
    double show_z() const {return coordinateReference.show_z();}
    double show_angle_x() const {return coordinateReference.show_angle_x();}
    double show_angle_z() const {return coordinateReference.show_angle_z();}
    void set_ocs_name(QString str){coordinateReference.set_ocs_name(str);}
    void set_x(double dou){coordinateReference.set_x(dou);}
    void set_y(double dou){coordinateReference.set_y(dou);}
    void set_z(double dou){coordinateReference.set_z(dou);}
    void set_angle_x(double dou){coordinateReference.set_angle_x(dou);}
    void set_angle_z(double dou){coordinateReference.set_angle_z(dou);}


    QString show_areaId() const {return positioningArea.show_areaId();}
    QString show_areaName() const {return positioningArea.show_areaName();}
    QString show_areaAddress()const {return positioningArea.show_areaAddress();}
    QString show_areaDescription() const {return positioningArea.show_areaDescription();}
    void set_positioningArea(QString str_1,QString str_2,QString str_3,QString str_4);


    double show_areaHorizAccuracy() const {return qualityInfo.show_areaHorizAccuracy();}
    double show_areaVertAccuracy() const {return qualityInfo.show_areaVertAccuracy();}
    void set_areaQualityInfo(double dou_1,double dou_2)
    {
        qualityInfo.set_areaHorizAccuracy(dou_1);
        qualityInfo.set_areaVertAccuracy(dou_2);
    }

    int show_minUpdateTime() const {return updateInterval.show_minUpdateTime();}
    int show_defaultUpdateTime() const {return updateInterval.show_defaultUpdateTime();}
    void set_updateInterval(double dou_1,double dou_2)
    {
        updateInterval.set_minUpdateTime(dou_1);
        updateInterval.set_defaultUpdateTime(dou_2);
    }

private:
    API_updateInterval updateInterval;           //定位时间间隔
    QString positioningTech="05";                //定位技术类型 默认05
    API_positioningArea positioningArea;         //定位服务的使用区域
    API_coordinateReference coordinateReference; //坐标参考系
    API_areaQualityInfo qualityInfo;             //定位区域的平均质量信息

    QJsonObject positioningOption;
};

/*********实时定位数据**************/
//室外数据
class API_outdoorLocationData
{
public:
    API_outdoorLocationData()=default;
    API_outdoorLocationData(double dou_1,double dou_2,QString time,API_coordinateReference api_1,
                             API_locationQualityInfo api_2);

    void set_x_y(double x,double y);
    QJsonObject show()const{return outdoorLocationData;}

private:
    double lat;         //室外纬度
    double log;         //室外纬度
    QString timestamp;  //定位时间
    API_coordinateReference coordinateReference;  //坐标参考系
    API_locationQualityInfo qualityInfo;          //室外定位质量信息

    QJsonObject outdoorLocationData;
};
//室内数据
class API_indoorLocationData
{
public:
    API_indoorLocationData()=default;
    API_indoorLocationData(double dou_1,double dou_2,QString time,API_coordinateReference api_1,
                           API_locationQualityInfo api_2);

    void set_data(double x,double y,QString str_1,QString str_2);
    QJsonObject show()const{return indoorLocationData;}
private:

    QString tagID;            //标签地址
    QString additionMessage;  //其他数据
    double coordx;         //室内坐标的 x
    double coordy;         //室内坐标的 y
    QString timestamp;     //定位时间
    API_coordinateReference coordinateReference;  //坐标参考系
    API_locationQualityInfo qualityInfo;          //室内定位质量信息

    QJsonObject indoorLocationData;
};

class udp_json_API
{
public:
    udp_json_API()=default;
    udp_json_API(API_version api_1,API_positioningOption api_2,API_outdoorLocationData api_3,
                 API_indoorLocationData api_4):version(api_1),positioningOption(api_2),
        outdoorLocationData(api_3),indoorLocationData(api_4){}

    void add_station(const API_stayion& sta)
    {
        stations.push_back(sta);
        station_num[sta.show_UUID()]=stations.size()-1;
    }
    void clear_station()
    {
        stations.clear();
    }
    QVector<float> show_stations()
    {
        QVector<float> result;
        for(auto&i:stations)
        {
            result.push_back(i.X());
            result.push_back(i.Y());
        }
        return result;
    }
    QVector<QString> show_UUIDs()
    {
        QVector<QString> result;
        for(auto &i:stations)
        {
             result.push_back(i.show_UUID());
        }
        return result;
    }
    int show_station_port(QString str)
    {
        return stations[station_num[str]].show_port();
    }
    float show_station_X(QString str)
    {
        return stations[station_num[str]].X();
    }
    float show_station_Y(QString str)
    {
        return stations[station_num[str]].Y();
    }
    void reset_ps()
    {
        QString str="主基站";
        float x1=0,y1=0;
        for(int i=0;i!=stations.size();++i)
        {
            if(stations[i].show_p_s()==str)
            {
                x1=stations[i].X();
                y1=stations[i].Y();
                break;
            }
            else
                continue;
        }
        for(int i=0;i!=stations.size();++i)
        {
            stations[i].set_distance(x1,y1);
        }

    }


    void set_version(API_version ver){version=ver;}
    void set_positioningOption(API_positioningOption ver){positioningOption=ver;}
    void set_outdoorLocationData(API_outdoorLocationData ver){outdoorLocationData=ver;}
    void set_indoorLocationData(API_indoorLocationData ver){indoorLocationData=ver;}

    void set_x_y(double x,double y,QString str_1,QString str_2)
    {    
        indoorLocationData.set_data(x,y,str_1,str_2);
        outdoorLocationData.set_x_y(x,y);
    }

    QJsonObject show_1_Capabilitie();
    QJsonObject show_2_Initialize();
    QJsonObject show_3_Start();
    QJsonObject show_4_Stop();

    QString show_ocs() {return positioningOption.show_ocs_name();}
    double show_coor_x() const {return positioningOption.show_x();}
    double show_coor_y() const {return positioningOption.show_y();}
    double show_coor_z() const {return positioningOption.show_z();}
    double show_coor_angle_x() const {return positioningOption.show_angle_x();}
    double show_coor_angle_z() const {return positioningOption.show_angle_z();}
    void set_coordinateReference(QString str,double dou,double dou_2,
                                 double dou_3,double dou_4,double dou_5);

    QString show_areaId() const {return positioningOption.show_areaId();}
    QString show_areaName()const {return positioningOption.show_areaName();}
    QString show_areaAddress() const {return positioningOption.show_areaAddress();}
    QString show_areaDescription() const {return positioningOption.show_areaDescription();}
    void set_positioningArea(QString str_1,QString str_2,QString str_3,QString str_4)
    {
        positioningOption.set_positioningArea(str_1,str_2,str_3,str_4);
    }

    double show_areaH() const {return positioningOption.show_areaHorizAccuracy();}
    double show_areaV() const {return positioningOption.show_areaVertAccuracy();}
    void set_areaQualityInfo(double dou_1,double dou_2)
    {
        positioningOption.set_areaQualityInfo(dou_1,dou_2);
    }

    int show_minUpdateTime() const {return positioningOption.show_minUpdateTime();}
    int show_defaultUpdateTime() const {return positioningOption.show_defaultUpdateTime();}
    void set_updateInterval(double dou_1,double dou_2)
    {
        positioningOption.set_updateInterval(dou_1,dou_2);
    }
private:
    API_version version;
    API_positioningOption  positioningOption;
    API_outdoorLocationData outdoorLocationData;
    API_indoorLocationData indoorLocationData;

    QVector<API_stayion> stations;   //数据库中包含的基站信息
    QMap<QString,int> station_num;    //uuid与存储位置的对应

    QString start_time;    //开始定位时间
};

#endif // UDP_JSON_API_H
