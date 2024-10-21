#include <bitset>
#include <cmath>
#include <cstdio>
#include <cfloat>

#include "autofile.h"

#define EPS 1e-12

enum hw_error_t
{
  NEITHER,
  ARITHMETIC,
  GEOMETRIC,
  CONSTANT,
  CANT_DECIDE,

  UNKNOWN_ERROR = -4,
  ARGS_ERR,
  FOPEN,
  WRONG_DATA
};

bool
equal (double a, double b)
{
  return fabs (a - b) < DBL_EPSILON * (fabs(a) > fabs(b) ? fabs(a) : fabs(b));
}

int
func (char *filename)
{
  autofile in (filename, "r");
  if (!in.is_open ())
    return FOPEN;

  double prev, next;
  if (fscanf (in.get (), "%lf%lf", &prev, &next) != 2)
    return WRONG_DATA;
  double diff[2] = { 0, 0 }, ratio[2] = { 0, 0 };
  ratio[0] = next / prev;
  diff[0] = next - prev;

  std::bitset<5> flags{ "11111" };
  if (!equal (prev, next))
    flags[CONSTANT] = false;
  prev = next;
  while (fscanf (in.get (), "%lf", &next) == 1)
    {
      ratio[1] = next / prev;
      diff[1] = next - prev;
      if (!equal (ratio[0], ratio[1]))
        flags[GEOMETRIC] = false;
      if (!equal (diff[0], diff[1]))
        flags[ARITHMETIC] = false;
      if (!equal (prev, next))
        flags[CONSTANT] = false;
      if ((int)flags[GEOMETRIC] + flags[ARITHMETIC] + flags[CONSTANT] <= 1)
        flags[CANT_DECIDE] = false;

      prev = next;
      ratio[0] = ratio[1];
      diff[0] = diff[1];
    }
  if (in.is_error ())
    return WRONG_DATA;

  if (flags[CANT_DECIDE])
    flags[GEOMETRIC] = flags[ARITHMETIC] = flags[CONSTANT] = false;
  if (flags.count () > 1)
    flags[NEITHER] = false;

  switch (flags.to_ulong ())
    {
    case 0b00001:
      return NEITHER;
    case 0b00010:
      return ARITHMETIC;
    case 0b00100:
      return GEOMETRIC;
    case 0b01000:
      return CONSTANT;
    case 0b10000:
      return CANT_DECIDE;
    }
  return UNKNOWN_ERROR;
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf ("Usage : %s filename\n", argv[0]);
      return ARGS_ERR;
    }
  switch (func (argv[1]))
    {
    case NEITHER:
      printf ("Sequence is neither an arithmetic nor a geometric progression "
              "nor a constant.\n");
      return 0;
    case ARITHMETIC:
      printf ("Sequence an arithmetic progression.\n");
      return 0;
    case GEOMETRIC:
      printf ("Sequence a geometric progression.\n");
      return 0;
    case CONSTANT:
      printf ("Sequence a constant.\n");
      return 0;
    case CANT_DECIDE:
      printf ("Unable to identify sequence type.\n");
      return 0;
    case FOPEN:
      printf ("Couldn't open file \"%s\" for read.\n", argv[1]);
      return FOPEN;
    case WRONG_DATA:
      printf ("Input file \"%s\" contains data in wrong format.\n", argv[1]);
      return WRONG_DATA;
    }
  return -1;
};