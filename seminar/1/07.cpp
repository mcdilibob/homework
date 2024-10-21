#include <cstdio>
#include <ctime>
#include <cmath>
#include <cfloat>

#include "autofile.h"

enum error_t
{
  OK = 0,
  ARGS_ERR = -4,
  FOPEN,
  WRONG_DATA,
  ALLOC_ERR
};

void input_array (double *a, int n, int s);
int read_array_from_file (double *a, int n, char *filename);
void print_array (double *a, int n, int p);
int func (double *a, int n, char *b);
bool equal (double a, double b);

int
main (int argc, char *argv[])
{
  int n = 0, p = 0, s = 0;
  if (argc < 5 || argc > 6)
    {
      printf ("Usage : %s filename n p s (filename)\n", argv[0]);
      return ARGS_ERR;
    }
  if (!sscanf (argv[2], "%d", &n)
      || !sscanf (argv[3], "%d", &p) || !sscanf (argv[4], "%d", &s))
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }

  if (n <= 0 || p < 0 || p > n || (s && argc == 6))
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }
  double *a = new double[n];
  if (!a)
    {
      printf ("Memory alloc error.\n");
      return ALLOC_ERR;
    }
  int ret;
  if (s)
    input_array (a, n, s);
  else if ((ret = read_array_from_file (a, n, argv[5])) != OK)
    {
      delete[] a;
      switch (ret)
        {
        case FOPEN:
          printf ("Couldn't open file \"%s\" for read.\n", argv[5]);
          return FOPEN;
        case WRONG_DATA:
          printf ("Input file \"%s\" contains data in wrong format.\n",
                  argv[5]);
          return WRONG_DATA;
        }
    }
  print_array (a, n, p);

  clock_t start = clock ();
  func (a, n, argv[1]);
  double time = (double)(clock () - start) / CLOCKS_PER_SEC;

  print_array (a, n, p);
  printf ("Elapsed time: %.2f", time);
}

double
f (int s, int n, int i)
{
  switch (s)
    {
    case 1:
      return i;
    case 2:
      return n - i;
    case 3:
      return i / 2.;
    case 4:
      return n - i / 2.;
    case 5:
      return 2 * i;
    case 6:
      return n - 2 * i;
    }
  return 0;
}

void
input_array (double *a, int n, int s)
{
  for (int i = 0; i < n; i++)
    a[i] = f (s, n, i);
}

int
read_array_from_file (double *a, int n, char *filename)
{
  autofile in (filename, "r");
  if (!in.is_open ())
    return FOPEN;
  for (int i = 0; i < n; i++)
    if (fscanf (in.get (), "%lf", a + i) != 1)
      return WRONG_DATA;
  if (!in.is_end ())
    return WRONG_DATA;
  return OK;
}

void
print_array (double *a, int n, int p)
{
  int size = p < n ? p : n;
  for (int i = 0; i < size; i++)
    printf ("%lf\n", a[i]);
}

bool
equal (double a, double b)
{
  return fabs (a - b) < DBL_EPSILON * (fabs(a) > fabs(b) ? fabs(a) : fabs(b));
}

int
func (double *a, int n, char *b)
{
  autofile in (b, "r");
  if (!in.is_open ())
    return FOPEN;

  double x;
  int i = 0, file_pos = 0, num_substr = 0;
  while (fscanf (in.get (), "%lf", &x) == 1)
    {
      if (equal(a[i], x))
      {
        i++;
        if (i == 2)
          file_pos = ftell (in.get ());
        if (i == n)
          {
            num_substr++;
            fseek (in.get (), 0, file_pos);
            i = 0;
          }
      }
      else
       {
        if (i == 0)
          continue;
        i = 0;
        fseek (in.get (), 0, file_pos);
       }
    }
  if (in.is_error ())
    return WRONG_DATA;
  return OK;
}