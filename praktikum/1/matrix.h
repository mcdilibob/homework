#ifndef MATRIX_H
#define MATRIX_H

#include <cstring>
#include <cmath>

// matrix functions
int inverse (double *A, double *X, int n, int m, double *buf0, double *buf1, double *buf2, double *D);
void identity_matrix (double *X, int n, int m);
void diff_mult (double *res, double *A, double *B, int n, int m);
double norm (double *A, int n, int m);
void swap (double *A, double *buf, int i, int j, int n, int m);
double residual (double *A, double *Inv, double *str, int n, int m);
double *get_block (double *A, int i, int j, int n, int m);

// single block functions
int inverse_block (double *block, double *inv, double *buf, int size);
void identity_block (double *block, int size);
void zero_block (double *block, int n, int m);
void mult_block (double *block1 /*v x t*/, double *block2 /*t x h*/,
                 double *res /*v x h*/, int v, int t, int h);
void diff_mult_block (double *res /*v x h*/, double *block1 /*v x t*/,
                      double *block2 /*t x h*/, int v, int t, int h);
double norm_block (double *block, int size);
void swap_block (double *block, double *buf, int i, int j, int size);

#endif