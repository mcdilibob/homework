#include "autofile.h"

#include <cstdio>
#include <ctime>

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
void func (double *a, int n, int k);

int
main (int argc, char *argv[])
{
  int k = 0, n = 0, p = 0, s = 0;
  if (argc < 5 || argc > 6)
    {
      printf ("Usage : %s k n p s (filename)\n", argv[0]);
      return ARGS_ERR;
    }
  if (!sscanf (argv[1], "%d", &k) || !sscanf (argv[2], "%d", &n)
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
  func (a, n, k);
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

int
gcd (int a, int b)
{
  return b != 0 ? gcd (b, a % b) : a;
}

void
func (double *a, int n, int k)
{
  if (k == 0)
    return;
  while (k < 0)
    k += n;
  k %= n;

  int m = gcd (n, k);
  for (int i = 0; i < m; i++)
    {
      double tmp = a[n - 1 - i];
      int j, l;
      for (j = l = i; 1; j = l)
        {
          l = j - k;
          if (l >= 0)
            l += n;
          if (l == i)
            break;
          a[j] = a[l];
        }
      a[j] = tmp;
    }
}