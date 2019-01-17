#include "udp_json_api.h"
#include <QDateTime>

/*********************   version函数   *******************/
API_version::API_version(QString str_1,QString str_2):availableVersion(str_1),currentVersion(str_2)
{
    version.insert("availableVersion",str_1);
    version.insert("currentVersion",str_2);
}


/*********************   positioningArea函数   *******************/
API_positioningArea::API_positioningArea(QString str_1,QString str_2,QString str_3,QString str_4):
    areaId(str_1),areaName(str_2),areaAddress(str_3),areaDescription(str_4),
positioningArea()
{
    positioningArea.insert("areaId",str_1);
    positioningArea.insert("areaName",str_2);
    positioningArea.insert("areaAddress",str_3);
    positioningArea.insert("areaDescription",str_4);
}


/*********************   ocs函数   *******************/
API_ocs::API_ocs(QString str_1,QString str_2):name(str_1),description(str_2)
{
    ocs.insert("name",str_1);
    ocs.insert("description",str_2);
}


/*********************   coordinateReference函数   *******************/
API_coordinateReference::API_coordinateReference(API_ocs ocs_1,double x_1,double y_1,double z_1,double an_x,double an_z):
    ocs(ocs_1), x(x_1),y(y_1),z(z_1),angle_x(an_x),angle_z(an_z)
{
    coordinateReference.insert("ocs",QJsonValue(ocs_1.show()));
    coordinateReference.insert("x",x_1);
    coordinateReference.insert("y",y_1);
    coordinateReference.insert("z",z_1);
    coordinateReference.insert("angle_x",an_x);
    coordinateReference.insert("angle_z",an_z);
}


/*********************   areaQualityInfo函数   *******************/
API_areaQualityInfo::API_areaQualityInfo(double dou_1,double dou_2,QString str):
    areaHorizAccuracy(dou_1),areaVertAccuracy(dou_2),unit(str)
{
    areaQualityInfo.insert("areaHorizAccuracy",dou_1);
    areaQualityInfo.insert("areaVertAccuracy",dou_2);
    areaQualityInfo.insert("unit",str);
}


/*********************   locationQualityInfo函数   *******************/
API_locationQualityInfo::API_locationQualityInfo(int num,double dou_1,double dou_2,QString str):
    qualityIndicator(num),horizAccuracy(dou_1),vertAccuracy(dou_2), unit(str)
{
    locationQualityInfo.insert("qualityIndicator",num);
    locationQualityInfo.insert("horizAccuracy",dou_1);
    locationQualityInfo.insert("vertAccuracy",dou_2);
    locationQualityInfo.insert("unit",str);
}


/*********************     locationQualityInfo函数   *******************/
API_updateInterval::API_updateInterval(int num,int num_2):
    minUpdateTime(num),defaultUpdateTime(num_2)
{
    updateInterval.insert("minUpdateTime",num);
    updateInterval.insert("defaultUpdateTime",num_2);
}



/*********************   positioningOption函数    *******************/
API_positioningOption::API_positioningOption(API_updateInterval api_1, QString str,API_positioningArea api_2,
API_coordinateReference api_3,API_areaQualityInfo api_4):updateInterval(api_1),positioningTech(str),positioningArea(api_2),
coordinateReference(api_3),qualityInfo(api_4)
{
    positioningOption.insert("updateInterval",QJsonValue(api_1.show()));
    positioningOption.insert("positioningTech",str);
    positioningOption.insert("positioningArea",QJsonValue(api_2.show()));
    positioningOption.insert("coordinateReference",QJsonValue(api_3.show()));
    positioningOption.insert("qualityInfo",QJsonValue(api_4.show()));
}

void API_positioningOption::set_positioningArea(QString str_1,QString str_2,QString str_3,QString str_4)
{
    positioningArea.set_areaId(str_1);
    positioningArea.set_areaName(str_2);
    positioningArea.set_areaAddress(str_3);
    positioningArea.set_areaDescription(str_4);
}

/*********************    outdoorLocationData函数    *******************/
API_outdoorLocationData::API_outdoorLocationData(double dou_1,double dou_2,QString time,API_coordinateReference api_1,
API_locationQualityInfo api_2):lat(dou_1),log(dou_2),timestamp(time),coordinateReference(api_1),qualityInfo(api_2)
{
    outdoorLocationData.insert("lat",dou_1);
    outdoorLocationData.insert("log",dou_2);
    outdoorLocationData.insert("locationTime",timestamp);
    outdoorLocationData.insert("coordinateReference",QJsonValue(api_1.show()));
    outdoorLocationData.insert("qualityInfo",QJsonValue(api_2.show()));
}

void API_outdoorLocationData::set_x_y(double x,double y)
{
    QDateTime time_now=QDateTime::currentDateTime();
    timestamp=time_now.toString("yyyyMMddThhmmss");

    outdoorLocationData.remove("lat");
    outdoorLocationData.remove("log");
    outdoorLocationData.remove("locationTime");
    outdoorLocationData.insert("lat",x+lat);
    outdoorLocationData.insert("log",y+log);
    outdoorLocationData.insert("locationTime",timestamp);
}


/*********************    indoorLocationData函数    *******************/
API_indoorLocationData::API_indoorLocationData(double dou_1,double dou_2,QString time,API_coordinateReference api_1,
API_locationQualityInfo api_2):coordx(dou_1),coordy(dou_2),timestamp(time),coordinateReference(api_1),qualityInfo(api_2)
{
    QString str="";
    indoorLocationData.insert("coordx",dou_1);
    indoorLocationData.insert("coordy",dou_2);
    indoorLocationData.insert("locationTime",timestamp);
    indoorLocationData.insert("coordinateReference",QJsonValue(api_1.show()));
    indoorLocationData.insert("qualityInfo",QJsonValue(api_2.show()));
    indoorLocationData.insert("tagID",QJsonValue(str));
    indoorLocationData.insert("additionMessage",QJsonValue(str));
}

void API_indoorLocationData::set_data(double x,double y,QString str_1,QString str_2)
{
    QDateTime time_now=QDateTime::currentDateTime();
    timestamp=time_now.toString("yyyyMMddThhmmss");

    indoorLocationData.remove("coordx");
    indoorLocationData.remove("coordy");
    indoorLocationData.remove("locationTime");
    indoorLocationData.remove("tagID");
    indoorLocationData.remove("additionMessage");

    indoorLocationData.insert("coordx",x);
    indoorLocationData.insert("coordy",y);
    indoorLocationData.insert("locationTime",timestamp);
    indoorLocationData.insert("tagID",QJsonValue(str_1));
    indoorLocationData.insert("additionMessage",QJsonValue(str_2));
}

/*********************    udp_json_API函数    *******************/

QJsonObject udp_json_API::show_1_Capabilitie()
{
    QJsonObject result;
    result=positioningOption.show();
    result.insert("version",QJsonValue(version.show()));
    return result;
}

QJsonObject udp_json_API::show_2_Initialize()
{
    QDateTime time_now=QDateTime::currentDateTime();
    start_time=time_now.toString("yyyyMMddThhmmss");

    QJsonObject result;
    result.insert("positioningOption",QJsonValue(positioningOption.show()));
    result.insert("clientIdentifier",QJsonValue("ab5456welkjs"));
    result.insert("clientState",QJsonValue("initialization"));
    result.insert("createdTime",QJsonValue(start_time));
    result.insert("currentVersion",QJsonValue(version.show_currentVersion()));
    return result;
}

QJsonObject udp_json_API::show_3_Start()
{
    QJsonObject result;
    result.insert("outdoorLocationData",QJsonValue(outdoorLocationData.show()));
    result.insert("indoorLocationData",QJsonValue(indoorLocationData.show()));
    return result;
}

QJsonObject udp_json_API::show_4_Stop()
{
    QDateTime time_now=QDateTime::currentDateTime();
    QString str=time_now.toString("yyyyMMddThhmmss");

    QJsonObject result;
    result.insert("clientIdentifier",QJsonValue("ab5456welkjs"));
    result.insert("clientState",QJsonValue("stop"));
    result.insert("createdTime",QJsonValue(start_time));
    result.insert("stoppedTime",QJsonValue(str));
    result.insert("currentVersion",QJsonValue(version.show_currentVersion()));
    return result;
}

void udp_json_API::set_coordinateReference
(QString str,double dou,double dou_2,double dou_3,double dou_4,double dou_5)
{
    positioningOption.set_ocs_name(str);
    positioningOption.set_x(dou);
    positioningOption.set_y(dou_2);
    positioningOption.set_z(dou_3);
    positioningOption.set_angle_x(dou_4);
    positioningOption.set_angle_z(dou_5);
}
