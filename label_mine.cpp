#include "label_mine.h"

/********Station_massage 函数***********/
//增加新数据
void Station_massage::add(const QByteArray& vec)
{
    vec_station.push_back(vec);
    if(vec_station.size()>20)
    {
        perpared=true;
        perpared=true;
    }
    perpared=true;
}

/********Label_mine 函数***********/
//寻找是否存在该基站
int Label_mine::find(int num)
{
    auto ptr=location_ID.find(num);
    if(ptr==location_ID.end())
        return -1;
    else
        return *ptr;
}

//增加新的基站
void Label_mine::add(int k,const Station_massage& vec)
{
    label.push_back(vec);
    location_ID[k]=num;
    num++;
}

//增加新的数据
void Label_mine::add_data(const QByteArray& vec,int num)
{
    label[num].add(vec);
}

//数据整合处理 若可进行定位，返回true
bool Label_mine::handing_data()
{
    int num=0;
    QVector<int> vec;

    //判断是否有四个基站存在数据
    for(int i=0;i!=label.size();++i)
    {
        if(label[i].perpare())
        {
            ++num;
            vec.push_back(i);
        }
    }

    //若有四个基站存在数据，
    if(num==4)
    {
        auto res_1=label[vec[0]].front();
        auto res_2=label[vec[1]].front();
        auto res_3=label[vec[2]].front();
        auto res_4=label[vec[3]].front();
        if((res_1==res_2)&&(res_2==res_3)&&(res_3==res_4))          //若序列号相同
        {
            result.clear();
            for(int i=0;i!=4;++i)                              //提取信息
            {
                result+=label[vec[i]].station_front();

                label[vec[i]].pop_front();                //清空最前端信息
                label[vec[i]].updata_perpared();          //重置perpared信息
            }
            return true;
        }
        else
        {
            auto res_5=label[vec[0]].back();
            auto res_6=label[vec[1]].back();
            auto res_7=label[vec[2]].back();
            auto res_8=label[vec[3]].back();

            //若序列号相同
            if((res_5==res_6)&&(res_6==res_7)&&(res_7==res_8))
            {
                result.clear();
                //提取信息，并清空基站内所有信息
                for(int i=0;i!=4;++i)
                {
                    result+=label[vec[i]].station_back();
                }
                clear();
                for(int i=0;i!=4;++i)
                {
                    label[vec[i]].set_perpare_false();
                }
                return true;
            }
            else
                return false;
        }
    }

    else
    {
        if(num>4)
        {
            //报错
        }
        return false;
    }
}

/************ Label_S函数 *************/

//寻找是否存在该标签
int Label_S::find(int num)
{
    auto ptr=labels_ID.find(num);
    if(ptr==labels_ID.end())
        return -1;
    else
        return *ptr;
}

//寻找是否存在该基站
int Label_S::find_station(int label_location,int station_num)
{
     return label_s[label_location].find(station_num);
}

//增加新的标签
void Label_S::add_new_label(const Label_mine& label,int k)
{
    label_s.push_back(label);
    labels_ID[k]=num;
    num++;
}

//增加新的基站 并处理
bool Label_S::add_new_station(const Station_massage& sta,int label_location,int station_num)
{
    //增加新的基站
    label_s[label_location].add(station_num,sta);

    //数据判断及处理
    return label_s[label_location].handing_data();
}

//增加新的数据 并处理
bool Label_S::add_new_data(const QByteArray& vec,int label_location,int station_location)
{
    //增加新的数据
    label_s[label_location].add_data(vec, station_location);

    //数据判断及处理
    return label_s[label_location].handing_data();
}
