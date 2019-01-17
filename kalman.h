//#ifndef KALMAN_H
//#define KALMAN_H

//#include <vector>
//#include <Eigen/Dense>
//#include <map>

//using namespace Eigen;

//class Kalman
//{
//public:
//    Kalman();
//    void new_label( int unsigned num)
//    {
//        Matrix4d x;
//        Vector4d p;
//        X.insert({num,x});
//        P.insert({num,p});
//    }

//    Matrix4d M_P(unsigned int num)
//    {
//        return  P[num];
//    }
//    Vector4d M_X(unsigned int num)
//    {
//        return  X[num];
//    }
//    const double& B_value()const{return B;}
//    const double& U_value()const{return u;}

//    void set_x(Matrix<double, 4, 1>x_,unsigned int num)
//    { X[num]=x_; }

//    void set_p(Matrix<double, 4, 4>p_,unsigned int num)
//    { P[num]=p_; }

//    void set_x_first(double& dou,unsigned int num)
//    {  P[num][0]=dou;}

//    void set_x_second(double& dou,unsigned int num)
//    {  P[num][1]=dou; }

//    void set_z_first(double& dou){ z[0]=dou;}
//    void set_z_second(double& dou){ z[1]=dou; }

//    Matrix<double, 4, 2>& M_K()
//    {
//        return K;
//    }
//    const Matrix<double, 2, 1>& M_Z()const
//    {
//        return  z;
//    }
//    const Matrix<double, 4, 4>& M_A()const
//    {
//        return A;
//    }
//    const Matrix<double, 4, 4>& M_Q()const
//    {
//        return  Q;
//    }
//    const Matrix<double, 2, 4>& M_H()const
//    {
//        return H;
//    }
//    const Matrix<double, 2, 2>& M_R()const
//    {
//        return  R;
//    }

//private:
//    double B;
//    double u;

//    Matrix<double, 4, 2> K;          //滤波增益方程

//    Matrix<double, 2, 1> z;          //观测值

//    Matrix<double, 4, 4> A;          //计算估计值矩阵
//    Matrix<double, 4, 4> Q;          //预估值得过程噪声

//    Matrix<double, 2, 4> H;          //系统噪声矩阵
//    Matrix<double, 2, 2> R;          //实际值过程噪声

//    std::map < unsigned int ,Vector4d,std::less<unsigned int>,
//    Eigen::aligned_allocator<std::pair<const unsigned int,Eigen::Vector4d>>> X;    //上一次的实际值
//    std::map<unsigned int ,Matrix4d > P;    //上一次的均方误差
//};

//#endif // KALMAN_H
