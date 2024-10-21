#include "autofile.h"

#include <cstdio>
#include <ctime>
#include <cfloat>

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
int func (double *a, double *b, double *c, int n, int m);
void free_mem (double *a, double *b, double *c);
bool equal (double a, double b);

int
main (int argc, char *argv[])
{
  int n = 0, m = 0, p_a = 0, p_b = 0, s_a = 0, s_b = 0;
  if (argc < 7 || argc > 9)
    {
      printf ("Usage : %s n p_a, s_a (filename_a) m p_b, s_b (filename_b)\n",
              argv[0]);
      return ARGS_ERR;
    }
  if (!sscanf (argv[1], "%d", &n) || !sscanf (argv[2], "%d", &p_a)
      || !sscanf (argv[3], "%d", &s_a))
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }

  if (n <= 0 || p_a < 0 || p_a > n || (s_a && argc == 7))
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }
  int t = s_a == 0;
  if (!sscanf (argv[4 + t], "%d", &m) || !sscanf (argv[5 + t], "%d", &p_b)
      || !sscanf (argv[6 + t], "%d", &s_b))
    {
      printf ("Wrong argument format.\n");
      return ARGS_ERR;
    }

  double *a = new double[n];
  double *b = new double[m];
  double *c = new double[n+m];
  if (!a || !b || !c)
    {
      free_mem (a, b, c);
      printf ("Memory alloc error.\n");
      return ALLOC_ERR;
    }
  int ret;
  if (s_a)
    input_array (a, n, s_a);
  else if ((ret = read_array_from_file (a, n, argv[4])) != OK)
    {
      free_mem (a, b, c);
      switch (ret)
        {
        case FOPEN:
          printf ("Couldn't open file \"%s\" for read.\n", argv[4]);
          return FOPEN;
        case WRONG_DATA:
          printf ("Input file \"%s\" contains data in wrong format.\n",
                  argv[4]);
          return WRONG_DATA;
        }
    }
  print_array (a, n, p_a);

  if (s_b)
    input_array (b, m, s_b);
  else if ((ret = read_array_from_file (b, m, argv[7+t])) != OK)
    {
      free_mem (a, b, c);
      switch (ret)
        {
        case FOPEN:
          printf ("Couldn't open file \"%s\" for read.\n", argv[7+t]);
          return FOPEN;
        case WRONG_DATA:
          printf ("Input file \"%s\" contains data in wrong format.\n",
                  argv[7+t]);
          return WRONG_DATA;
        }
    }
  print_array (b, m, p_b);


  clock_t start = clock ();
  int l = func (a, b, c, n, m);
  double time = (double)(clock () - start) / CLOCKS_PER_SEC;

  print_array (c, l, p_a);
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

void free_mem (double *a, double *b, double *c)
{
  delete[] a;
  delete[] b;
  delete[] c;
}

void
print_array (double *a, int n, int p)
{
  int size = p < n ? p : n;
  for (int i = 0; i < size; i++)
    printf ("%lf\n", a[i]);
}


int
func (double *a, double *b, double *c, int n, int m)
{
  int i, j, k;
  for (i = j = k = 0; i < n && j < m;)
    {
      if (equal(a[i], b[j]))
        i++;
      else if (a[i] < b[j])
        c[k++] = a[i++];
      else if (a[i] > b[j])
        c[k++] = b[j++];
    }
    while (i < n)
      c[k++] = a[i++];
    while (j < m)
      c[k++] = b[j++];

    return k;
}