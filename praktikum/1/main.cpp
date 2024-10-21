#include "matrix.h"
#include "matrix_io.h"
#include <ctime>

void free_mem (double *A, double *X, double *buf0, double *buf1, double *buf2,
               double *D, double *str);

int
main (int argc, char *argv[])
{
  int task = 1;
  int ret = OK;
  int n = 0, m = 0, r = 0, s = 0;
  char filename[FILENAME_MAX];
  if (argc < 5 || argc > 6)
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }
  n = atoi (argv[1]), m = atoi (argv[2]), r = atoi (argv[3]),
  s = atoi (argv[4]);
  if (n <= 0 || m <= 0 || m > n || r < 0 || (s && argc == 6))
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }
  if (argc == 6)
    strcpy (filename, argv[5]);

  double *A = new double[n * n];
  double *Inv = new double[n * n];
  double *buf0 = new double[m * m];
  double *buf1 = new double[m * m];
  double *buf2 = new double[n * m];
  double *D = new double[m * m];
  double *str = new double[n];
  if (!A || !Inv || !buf0 || !buf1 || !buf2 || !D || !str)
    {
      free_mem (A, Inv, buf0, buf1, buf2, D, str);
      printf ("Memory alloc error.\n");
      return ALLOC_ERR;
    }

  if (s)
    input_matrix (A, s, n, m);
  else
    {
      ret = read_matrix_from_file (A, filename, n, m);
      switch (ret)
        {
        case FOPEN:
          printf ("Couldn't open file.\n");
          break;
        case WRONG_DATA:
          printf ("Wrong data format.\n");
          break;
        }
    }

  if (ret == OK)
    {
      print_matrix (A, n, n, m, r);

      double r1 = -1, r2 = -1, t1 = 0, t2 = 0;
      clock_t start = clock ();
      ret = inverse (A, Inv, n, m, buf0, buf1, buf2, D);
      t1 = (double)(clock () - start) / CLOCKS_PER_SEC;

      if (ret == OK)
        {
          if (s)
            input_matrix (A, s, n, m);
          else
            read_matrix_from_file (A, filename, n, m);
          print_matrix (Inv, n, n, m, r);
          start = clock ();
          r1 = residual (A, Inv, str, n, m);
          r2 = residual (Inv, A, str, n, m);
          t2 = (double)(clock () - start) / CLOCKS_PER_SEC;
        }

      printf (
          "%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = "
          "%d M = %d\n",
          argv[0], task, r1, r2, t1, t2, s, n, m);
    }

  free_mem (A, Inv, buf0, buf1, buf2, D, str);
  return ret;
}

void
free_mem (double *A, double *Inv, double *buf0, double *buf1, double *buf2,
          double *D, double *str)
{
  delete[] A;
  delete[] Inv;
  delete[] buf0;
  delete[] buf1;
  delete[] buf2;
  delete[] D;
  delete[] str;
}