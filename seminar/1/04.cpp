#include <cmath>
#include <cstdio>

#include "autofile.h"

enum error_t
{
  OK = 0,
  ARGS_ERR = -3,
  FOPEN,
  WRONG_DATA
};

int
func (char *filename, double *d)
{
  autofile in (filename, "r");
  if (!in.is_open ())
    return FOPEN;
  int n = 0;
  double x, mean = 0, mse = 0;
  while (fscanf (in.get (), "%lf", &x) == 1)
    {
      double d1 = (x - mean);
      n++;
      mean += (x - mean) / n;
      double d2 = (x - mean);
      mse += (d1 * d2 - mse) / n;
    }
  if (in.is_error ())
    return WRONG_DATA;
  *d = sqrt (mse);
  return n;
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf ("Usage : %s filename\n", argv[0]);
      return ARGS_ERR;
    }
  double d;
  switch (func (argv[1], &d))
    {
    case FOPEN:
      printf ("Couldn't open file \"%s\" for read.\n", argv[1]);
      return FOPEN;
    case WRONG_DATA:
      printf ("Input file \"%s\" contains data in wrong format.\n", argv[1]);
      return WRONG_DATA;
    default:
      printf ("Mean squared error = %lf.\n", d);
      return OK;
    }
}