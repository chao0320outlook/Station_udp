#ifndef TAYLORLIB_H
#define TAYLORLIB_H

#ifndef TAYLORLIB_GLOBAL_H
#define TAYLORLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TAYLORLIB_LIBRARY)
#  define TAYLORLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TAYLORLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TAYLORLIB_GLOBAL_H

#include "QVector"
#include "matlib.h"

struct NewPoint
{
    double x;     //x坐标
    double y;     //y坐标
};


class TAYLORLIBSHARED_EXPORT Taylorlib
{
private:
    QVector<NewPoint> statio;
    NewPoint Base_station[4];           //基站坐标信息
    NewPoint center_mine;               //中心点

    NewPoint x0y0={0,0};               //实时位置信息
    NewPoint xy[10];                    //记录上一次定位值

public:
    Taylorlib();
    NewPoint Taylor(QVector<int> vec, QVector<NewPoint> station);
    double distance_sqr(const NewPoint & a, const NewPoint & b);
};

#endif // TAYLORLIB_H
