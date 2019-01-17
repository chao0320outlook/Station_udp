#ifndef LABEL_MINE_H
#define LABEL_MINE_H

#include <QVector>
#include <QList>
#include <QHash>
#include <QMap>

class Station_massage
{
public:
    Station_massage(bool flag=false):perpared(flag){}

    bool perpare() const {return perpared;}
    int front() const {return vec_station.front().at(2);}
    int back() const {return vec_station.back().at(2);}

    const QByteArray& station_front()const{return vec_station.front();}
    const QByteArray& station_back()const{return vec_station.back();}

    void pop_front(){vec_station.pop_front();}
    void pop_back(){vec_station.pop_back();}
    void clear(){vec_station.clear();}
    void updata_perpared()
    {
        if(vec_station.isEmpty())
            perpared=false;
    }
    void set_perpare_false(){perpared=false;}
    void add(const QByteArray& vec);
private:

    bool perpared;          //该基站有可处理数据
    QList<QByteArray> vec_station;
};

class Label_mine
{
public:

    Label_mine(int i=0):num(i){}

    //寻找是否存在该基站
    int find(int num);
    //增加新的基站
    void add(int k,const Station_massage& vec);
    //增加新的数据
    void add_data(const QByteArray& vec,int num);
    //数据整合处理
    bool handing_data();

    QByteArray new_location()const{return result;}
    void clear()
    {
        for(int i=0;i!=label.size();++i)
            label[i].clear();
    }
private:
    int num;
    QHash<int,int> location_ID;           //基站ID与存储顺序的对应
    QVector<Station_massage> label;
    QByteArray result;
};

class Label_S
{
public:
    Label_S(int i=0):num(i){}
    //寻找是否存在该标签
    int find(int num);
    //寻找是否存在该基站
    int find_station(int label_location,int station_num);
    //增加新的标签
    void add_new_label(const Label_mine& label,int k);
    //增加新的基站 并处理
    bool add_new_station(const Station_massage& sta,int label_location,int station_num);
    //增加新的数据 并处理
    bool add_new_data(const QByteArray& vec,int label_location,int station_location);
    //返回处理结果
    QByteArray new_location(int i) const {return label_s[i].new_location();}
    //清空存储
    void clear(){labels_ID.clear();label_s.clear();num=0;}

private:
    int num;
    QHash<int,int> labels_ID;           //标签ID与存储顺序的对应
    QVector<Label_mine> label_s;
};


#endif // LABEL_MINE_H
