#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QOpenGLWidget>
#include <memory>
#include <QDebug>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QVector>
#include <QVector3D>
#include <QMap>


#include <QMatrix4x4>
#include "transform3d.h"
#include "camera.h"

using namespace std;

class openglwindow : public QOpenGLWidget,
                     protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    openglwindow(QWidget *parent = 0);
    ~openglwindow();

    Camera3D m_camera;                    //相机

    QVector3D vec1[3]=
    {
        {1.0f,0.0f,0.0f},
        {0.0f,0.0f,1.0f},
        {1.0f,0.0f,1.0f},
    };

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void teardownGL();

    //设置相机移动
    void set_camera_up_true (){camera_up=true;}
    void set_camera_up_false (){camera_up=false;}
    void set_camera_back_true (){camera_back=true;}
    void set_camera_back_false (){camera_back=false;}
    void set_camera_left_true (){camera_left=true;}
    void set_camera_left_false (){camera_left=false;}
    void set_camera_right_true (){camera_right=true;}
    void set_camera_right_false (){camera_right=false;}

    void set_camera_move_true(){camera_move=true;}
    void set_camera_move_false(){camera_move=false;}

    void set_coccer_start()
    {
        if(soccer_start)
            soccer_start=false;
        else
            soccer_start=true;
    }
    void set_reset();
    void updata_plarer(unsigned int ID,double x,double y);

protected slots:
    void update();

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);


private:
    //shared_ptr<QOpenGLShaderProgram>m_program;
    QOpenGLBuffer m_vertex[2];                    //缓冲区
    QOpenGLVertexArrayObject m_object[2];
    QOpenGLShaderProgram *m_program;           //着色器
    QOpenGLTexture *texture[11];                //纹理

    // Shader Information
    int u_modelToWorld;                    //世界坐标
    int u_worldToCamera;                   //
    int u_cameraToView;

    QMap <unsigned int ,int> map1;              //存储 标号与模型序列 对应关系

    //相机移动状态
    bool camera_up =false;
    bool camera_back=false;
    bool camera_left=false;
    bool camera_right=false;
    bool camera_move =false;

    QString camera_coordinate;

    QMatrix4x4 m_projection;              //变换矩阵
    //Camera3D m_camera;                  //相机

    Transform3D m_transform[6];           //变换  0为球场  1-5为球员

    //球员初始位置
    QVector3D vec[5]=
    {
        {0.12f,0.0f,0.0f},      //中间
        {-5.21f,0.0f, 3.24f},   //左下
        {-5.21f,0.0f,-3.24f},   //左上
        { 5.45f,0.0f, 3.24f},   //右下
        { 5.45f,0.0f,-3.24f}    //右上
    };

    bool soccer_start=false;

    void printVersionInformation();
};

#endif // OPENGLWINDOW_H
