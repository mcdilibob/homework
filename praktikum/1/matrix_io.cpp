#include "matrix_io.h"
#include "matrix.h"

int
max (int a, int b)
{
  return a > b ? a : b;
}

double
f (int s, int n, int i, int j)
{
  i++, j++;
  switch (s)
    {
    case 1:
      return n - max (i, j) + 1;
    case 2:
      return max (i, j);
    case 3:
      return abs (i - j);
    case 4:
      return 1. / (i + j - 1);
    }
  return 0;
}

void
input_matrix (double *A, int s, int n, int m)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  for (int p = 0; p < bl; p++)
    for (int q = 0; q < bl; q++)
      {
        double *p_block = get_block (A, p, q, n, m);
        int v = p < k ? m : l, h = q < k ? m : l; // block size
        for (int i = 0; i < v; i++)
          for (int j = 0; j < h; j++)
            p_block[i * h + j] = f (s, n, p * m + i, q * m + j);
      }
}

int
read_matrix_from_file (double *A, char *filename, int n, int m)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  double x = 0;
  int ret = OK;
  FILE *in = fopen (filename, "r");
  if (!in)
    return FOPEN;
  for (int str_ind = 0; str_ind < n; str_ind++)
    {
      int p = str_ind / m;
      for (int q = 0; q < bl; q++)
        {
          double *p_block = get_block (A, p, q, n, m);
          int h = q < k ? m : l; // block size
          int i = str_ind % m;
          for (int j = 0; j < h; j++)
            {
              if (fscanf (in, "%lf", &x) != 1)
                {
                  ret = WRONG_DATA;
                  goto close_file;
                }
              p_block[i * h + j] = x;
            }
        }
    }
close_file:
  fclose (in);
  return ret;
}

void
print_matrix (double *A, int t, int n, int m, int r)
{
  int max_rows = t < r ? t : r;
  int max_cols = n < r ? n : r;
  int k = max_cols / m;
  int l = max_cols - k * m;
  int bl = l ? k + 1 : k;
  for (int str_ind = 0; str_ind < max_rows; str_ind++)
    {
      int p = str_ind / m;
      for (int q = 0; q < bl; q++)
        {
          double *p_block = get_block (A, p, q, n, m);
          int h1 = q < n / m ? m : n % m, h = q < k ? m : l; // block size
          int i = str_ind % m;
          for (int j = 0; j < h; j++)
            printf (" %10.3e", p_block[i * h1 + j]);
        }
      printf ("\n");
    }
  printf ("\n");
}