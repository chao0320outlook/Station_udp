#ifndef SOCCER_PLAYER_H
#define SOCCER_PLAYER_H

#include <QQueue>
#include <QVector>
#include <QString>
#include <QLabel>
#include <QByteArray>

class Soccer_player
{
public:
    Soccer_player();
    Soccer_player(QString name_1,int num_1,int num_s_1,QByteArray photo_1,int age_1=22 ,int weight_1=175,int height_1=65):
       name(name_1),num_s(num_s_1),age(age_1),weight(weight_1),height(height_1),number(num_1),photo(photo_1){}

    int show_mun_s(){return num_s;}
    int show_age(){return age;}
    int show_weight(){return weight;}
    int show_height(){return height;}
    int show_number(){return number;}
    QString show_name(){return name;}
    QByteArray show_photo(){return photo;}


    QString show_data();
private:

    QString name;
    int num_s;                    //球员佩戴的标签编号
    int age;
    int weight;
    int height;
    int number;                      //球员编号
    QByteArray photo;
};

#endif // SOCCER_PLAYER_H
