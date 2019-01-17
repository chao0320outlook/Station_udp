#ifndef MATLIB_H
#define MATLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void    MatShow_S(double* Mat, int row, int col);
double* MatAdd_S(double* A, double* B, int row, int col);
double* MatSub_S(double* A, double* B, int row, int col);
double* MatMul_S(double* A, int Arow, int Acol, double* B, int Brow, int Bcol);
double* MatMulk_S(double *A, int row, int col, double k);
double* MatT_S(double* A, int row, int col);
double  MatDet_S(double *A, int row);
double* MatInv_S(double *A, int row, int col);
double  MatACof_S(double *A, int row, int m, int n);
double* MatAdj_S(double *A, int row, int col);
double *MatRead_S(char *csvFileName);
void MatWrite_S(const char *csvFileName, double *A, int row, int col);


extern struct Mat
{
    int row, col;
    float **element;
}Mat;




#endif // MATLIB_H
