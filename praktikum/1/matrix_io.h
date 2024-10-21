#ifndef MATRIX_IO_H
#define MATRIX_IO_H

#include <cstdio>
#include <cstring>
#include <cstdlib>

enum err_t
{
  OK,
  ARGS_ERR,
  FOPEN,
  WRONG_DATA,
  INAPPLICABLE,
  ALLOC_ERR
};

double f (int s, int n, int i, int j);
void input_matrix (double *A, int s, int n, int m);
int read_matrix_from_file (double *A, char *filename, int n, int m);
void print_matrix (double *A, int t, int n, int m, int r);

#endif