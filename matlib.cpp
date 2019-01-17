#include "matlib.h"


// (det用)功能：求逆序对的个数
int inver_order_S(int list[], int n)
{
    int ret = 0;
    int i;
    int j;
    for ( i = 1; i < n; i++)
        for ( j = 0; j < i; j++)
            if (list[j] > list[i])
                ret++;
    return ret;
}

// (det用)功能：符号函数，正数返回1，负数返回-1
int sgn_S(int order)
{
    return order % 2 ? -1 : 1;
}

// (det用)功能：交换两整数a、b的值
void swap_S(int *a, int *b)
{
    int m;
    m = *a;
    *a = *b;
    *b = m;
}

// 功能：求矩阵行列式的核心函数
double det_S(double *p, int n, int k, int list[], double sum)
{
    int i;
    if (k >= n)
    {
        int order = inver_order_S(list, n);
        double item = (double)sgn_S(order);
        for ( i = 0; i < n; i++)
        {
            //item *= p[i][list[i]];
            item *= *(p + i * n + list[i]);
        }
        return sum + item;
    }
    else
    {
        for (i = k; i < n; i++)
        {
            swap_S(&list[k], &list[i]);
            sum = det_S(p, n, k + 1, list, sum);
            swap_S(&list[k], &list[i]);
        }
    }
    return sum;
}

// 功能：矩阵显示
// 形参：(输入)矩阵首地址指针Mat，矩阵行数row和列数col。
// 返回：无
void MatShow_S(double* Mat, int row, int col)
{
    int i;
    for ( i = 0; i < row*col; i++)
    {
        printf("%16lf ", Mat[i]);
        if (0 == (i + 1) % col) printf("\n");
    }
}

// 功能：矩阵相加
// 形参：(输入)矩阵A首地址指针A，矩阵B首地址指针B，矩阵A(也是矩阵B)行数row和列数col
// 返回：A+B
double* MatAdd_S(double* A, double* B, int row, int col)
{
    int i;
    int j;
    double *Out = (double*)malloc(sizeof(double) * row * col);
    for ( i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            Out[col*i + j] = A[col*i + j] + B[col*i + j];
    return Out;
}

// 功能：矩阵相减
// 形参：(输入)矩阵A，矩阵B，矩阵A(也是矩阵B)行数row和列数col
// 返回：A-B
double* MatSub_S(double* A, double* B, int row, int col)
{
    int i;
    int j;
    double *Out = (double*)malloc(sizeof(double) * row * col);
    for ( i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            Out[col*i + j] = A[col*i + j] - B[col*i + j];
    return Out;
}

// 功能：矩阵相乘
// 形参：(输入)矩阵A，矩阵A行数row和列数col，矩阵B，矩阵B行数row和列数col
// 返回：A*B
double* MatMul_S(double* A, int Arow, int Acol, double* B, int Brow, int Bcol)
{
    double *Out = (double*)malloc(sizeof(double) * Arow * Bcol);
    if (Acol != Brow)
    {
        printf("        Shit!矩阵不可乘!\n");
        return NULL;
    }
    else
    {
    int i, j, k;
    for (i = 0; i < Arow; i++)
        for (j = 0; j < Bcol; j++)
        {
            Out[Bcol*i + j] = 0;
            for (k = 0; k < Acol; k++)
                Out[Bcol*i + j] = Out[Bcol*i + j] + A[Acol*i + k] * B[Bcol*k + j];
        }
    return Out;
    }
}

// 功能：矩阵数乘(实数k乘以矩阵A)
// 形参：(输入)矩阵A首地址指针，矩阵行数row和列数col，实数k
// 返回：kA
double* MatMulk_S(double *A, int row, int col, double k)
{
    int i;
    double *Out = (double*)malloc(sizeof(double) * row * col);
    for (i = 0; i < row * col; i++)
    {
        *Out = *A * k;
        Out++;
        A++;
    }
    Out = Out - row * col;
    return Out;
}

// 功能：矩阵转置
// 形参：(输入)矩阵A首地址指针A，行数row和列数col
// 返回：A的转置
double* MatT_S(double* A, int row, int col)
{
    int i;
    int j;
    double *Out = (double*)malloc(sizeof(double) * row * col);
    for ( i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            Out[row*j + i] = A[col*i + j];
    return Out;
}

// 功能：求行列式值
// 形参：(输入)矩阵A首地址指针A，行数row
// 返回：A的行列式值
double MatDet_S(double *A, int row)
{
    int i ;
    double Out;
    int *list = (int*)malloc(sizeof(int) * row);
    for (i= 0; i < row; i++)
        list[i] = i;
    Out = det_S(A, row, 0, list, 0.0);
    free(list);
    return Out;
}
// 功能：矩阵的逆
// 形参：(输入)矩阵A首地址指针A，行数row和列数col
// 返回：A的逆
double *MatInv_S(double *A, int row, int col)
{
    int i;
    int len;
    double *Out = (double*)malloc(sizeof(double) * row * col);
    double det = MatDet_S(A, row); //求行列式
    if (det == 0)
    {
        printf("        Fuck!矩阵不可逆!\n");
        return NULL;
    }
    else
    {
        Out = MatAdj_S(A, row, col); //求伴随矩阵
        len = row * row;
        for ( i = 0; i < len; i++)
            *(Out + i) /= det;
        return Out;
    }
}

// 功能：求代数余子式
// 形参：(输入)矩阵A首地址指针A，矩阵行数row, 元素a的下标m，n(从0开始)，
// 返回：NxN 矩阵中元素A(mn)的代数余子式
double MatACof_S(double *A, int row, int m, int n)
{
    int i;
    double ret;
    int len = (row - 1) * (row - 1);
    double *cofactor = (double*)malloc(sizeof(double) * len);

    int count = 0;
    int raw_len = row * row;
    for ( i = 0; i < raw_len; i++)
        if (i / row != m && i % row != n)
            *(cofactor + count++) = *(A + i);
    ret = MatDet_S(cofactor, row - 1);
    if ((m + n) % 2)
        ret = -ret;
    free(cofactor);
    return ret;
}

// 功能：求伴随矩阵
// 形参：(输入)矩阵A首地址指针A，行数row和列数col
// 返回：A的伴随矩阵
double *MatAdj_S(double *A, int row, int col)
{
    int i;
    double *Out = (double*)malloc(sizeof(double) * row * col);
    int len = row * row;
    int count = 0;
    for (i = 0; i < len; i++)
    {
        *(Out + count++) = MatACof_S(A, row, i % row, i / row);
    }
    return Out;
}

// 读取文件行数
int FileReadRow_S(const char *filename)
{
    FILE *f = fopen(filename, "r");
    int i = 0;
    char str[4096];
    while (NULL != fgets(str, 4096, f))
        ++i;
    printf("数组行数：%d\n", i);
    return i;
}

// 读取文件每行数据数(逗号数+1)
int FileReadCol_S(const char *filename)
{
    FILE *f = fopen(filename, "r");
    int i = 0;
    int j;
    char str[4096];
    fgets(str, 4096, f);
    for (j = 0; j < strlen(str); j++)
    {
        if (',' == str[j]) i++;
    }
    i++;// 数据数=逗号数+1
    printf("数组列数：%d\n", i);
    return i;
}

// 逗号间隔数据提取
void GetCommaData_S(char str_In[4096], double double_Out[1024])
{
    int i;
    int str_In_len = strlen(str_In);
    //printf("str_In_len:%d\n", str_In_len);
    char str_Data_temp[128];
    int j = 0;
    int double_Out_num = 0;
    for (i = 0; i < str_In_len; i++)
    {
        //不是逗号，则是数据，存入临时数组中
        if (',' != str_In[i]) str_Data_temp[j++] = str_In[i];
        //是逗号或\n(最后一个数据)，则数据转换为double，保存到输出数组
        if (',' == str_In[i] || '\n' == str_In[i]) { str_Data_temp[j] = '\0'; j = 0; /*printf("str_Data_temp:%s\n", str_Data_temp); */double_Out[double_Out_num++] = atof(str_Data_temp); memset(str_Data_temp, 0, sizeof(str_Data_temp)); }
    }
}

// 功能：从csv文件读矩阵，保存到指针中
// 形参：(输入)csv文件名，预计行数row和列数col
// 返回：矩阵指针A
double *MatRead_S(char *csvFileName)
{
    int i;
    int row = FileReadRow_S(csvFileName);
    int col = FileReadCol_S(csvFileName);
    double *Out = (double*)malloc(sizeof(double) * row * col);
    FILE *f = fopen(csvFileName, "r");
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), f))
    {
        //printf("buffer[%s]\n",buffer);
        double double_Out[128] = { 0 };
        GetCommaData_S(buffer, double_Out);
        for (i = 0; i < col; i++)
        {
            //printf("double_Out:%lf\n", double_Out[i]);
            *Out = double_Out[i];
            Out++;
        }

    }
    Out = Out - row * col;//指针移回数据开头
    fclose(f);
    return Out;
}

// 功能：将矩阵A存入csv文件中
// 形参：(输入)保存的csv文件名，矩阵A首地址指针A，行数row和列数col
// 返回：无
void MatWrite_S(const char *csvFileName, double *A, int row, int col)
{
    int i;
    FILE *DateFile;
    double *Ap = A;
    DateFile = fopen(csvFileName, "w");//追加的方式保存生成的时间戳
    for (i = 0; i < row*col; i++)
    {
        if ((i+1) % col == 0) fprintf(DateFile, "%lf\n", *Ap);//保存到文件，到列数换行
        else fprintf(DateFile, "%lf,", *Ap);//加逗号
        Ap++;
    }
    fclose(DateFile);
}
