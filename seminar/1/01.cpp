#include "autofile.h"
#include <bitset>
#include <cstdio>

enum err_t
{
  OK = 0,
  ARGS_ERR = -3,
  FOPEN,
  WRONG_DATA,
  NO_SUCH_SEQ = __INT_MAX__
};

int
func (char *filename, int k)
{
  autofile in (filename, "r");
  if (!(in.is_open ()))
    return FOPEN;
  int x, min_distance = __INT_MAX__;
  int start = 0, end = 0, ind = 0;
  bool ones_flag = false;
  while (fscanf (in.get (), "%d", &x) == 1)
    {
      if (x == 0) // zero is processed as one zero
        {
          if (ones_flag)
            if (ind - start >= k)
            {
              if (end && start - end < min_distance)
                min_distance = start - end;
              end = ind;
            }
          ones_flag = false;
          ind++;
          continue;
        }
      std::bitset<8 * __SIZEOF_INT__> bitstr (x);
      int i = bitstr.size ();
      for (; i >= 0 && !bitstr[i]; i--) // skip leading zeros
        ;
      for (; i >= 0; i--, ind++)
        {
          if (bitstr[i] && !ones_flag)
            {
              ones_flag = true;
              start = ind;
            }
          else if (!bitstr[i] && ones_flag)
            {
              ones_flag = false;
              if (ind - start >= k)
                {
                  if (end && start - end < min_distance)
                    min_distance = start - end;
                  end = ind;
                }
            }
        }
    }
  if (ones_flag)
    if (ind - start >= k)
      if (end && start - end < min_distance)
        min_distance = start - end;
  if (in.is_error ())
    return WRONG_DATA;
  return min_distance;
}

int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      printf ("Usage : %s filename number\n", argv[0]);
      return ARGS_ERR;
    }
  int k = strtol (argv[2], nullptr, 10);
  if (k == 0)
    {
      printf ("Second argument must be a positive number.\n");
      return ARGS_ERR;
    }
  int ret = func (argv[1], k);
  switch (ret)
    {
    case FOPEN:
      printf ("Couldn't open file \"%s\" for read.\n", argv[1]);
      return FOPEN;
    case WRONG_DATA:
      printf ("Input file \"%s\" contains data in wrong format.\n", argv[1]);
      return WRONG_DATA;
    case NO_SUCH_SEQ:
      printf (
          "Couldn't find two sequences of 1-s with lengths greater than %d.\n",
          k);
      return OK;
    default:
      printf ("Minimal distance = %d.\n", ret);
      return OK;
    }
}