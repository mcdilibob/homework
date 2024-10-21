#include <cstdio>

#include "autofile.h"

enum error_t
{
  NO = 0,
  YES = 1,
  ARGS_ERR = -3,
  FOPEN,
  WRONG_DATA
};

int
func (char *filename1, char *filename2)
{
  autofile in1 (filename1, "r"), in2 (filename2, "r");
  int a[3] = { 0, 0, 0 };
  int b = 0;
  bool flag = 1;
  if (!in1.is_open () || !in2.is_open ())
    return FOPEN;
  fscanf (in1.get (), "%d%d", a, a + 1);
  fscanf (in2.get (), "%d", &b); // skip first element of b
  while (fscanf (in1.get (), "%d", a + 2) == 1
         && fscanf (in2.get (), "%d", &b) == 1)
    {
      if (b != (a[0] + a[2]) / 2)
        flag = 0;
      a[0] = a[1], a[1] = a[2];
    }
  fscanf (in2.get (), "%d", &b); // skip last element of b
  if (in1.is_error () || in2.is_error ())
    return WRONG_DATA;
  if (in1.is_end ())
    {
      if (flag && in2.is_end ())
        return YES;
      else
        return NO;
    }
  return NO;
}

int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      printf ("Usage : %s filename1 filename2\n", argv[0]);
      return ARGS_ERR;
    }
  switch (func (argv[1], argv[2]))
    {
    case YES:
      printf ("Yes\n");
      return 0;
    case NO:
      printf ("No\n");
      return 0;
    case FOPEN:
      printf ("Couldn't open files \"%s\", \"%s\" for read.\n", argv[1],
              argv[2]);
      return FOPEN;
    case WRONG_DATA:
      printf ("Input files \"%s\", \"%s\" contain data in wrong format.\n",
              argv[1], argv[2]);
      return WRONG_DATA;
    }
  return 1;
}