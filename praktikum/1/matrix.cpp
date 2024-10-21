#include "matrix.h"
#include "matrix_io.h"

#define EPS 1e-12

/* void
debug_print (double *A, double *Inv, int n, int m)
{
  printf ("Mat:\n-------------\n");
  print_matrix (A, n, n, m, n);
  printf ("Inv:\n-------------\n");
  print_matrix (Inv, n, n, m, n);
  printf ("-------------\n");
} */

// matrix functions
int
inverse (double *A, double *Inv, int n, int m, double *buf0, double *buf1,
         double *buf2, double *D)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  double *p_block = nullptr;
  identity_matrix (Inv, n, m);
  double norm_A = norm (A, n, m);
  for (int s = 0; s < bl; s++)
    { // pivot
      if (s != k)
        {
          double min_norm = __DBL_MAX__;
          int pivot = -1;
          for (int i = s; i < k; i++)
            {
              p_block = get_block (A, i, s, n, m);
              memcpy (buf0, p_block, m * m * sizeof (double));
              double cur_norm = -1;
              if (inverse_block (buf0, buf1, buf2, m) == INAPPLICABLE)
                continue;
              cur_norm = norm_block (buf1, m);
              if (cur_norm < min_norm)
                {
                  memcpy (D, buf1, m * m * sizeof (double));
                  min_norm = cur_norm;
                  pivot = i;
                }
            }
          if (min_norm < EPS / norm_A || min_norm > 1e300)
            return INAPPLICABLE;
          if (pivot != s)
            {

              swap (A, buf2, pivot, s, n, m);
              swap (Inv, buf2, pivot, s, n, m);
            }
        }
      else
        {
          p_block = get_block (A, k, k, n, m);
          memcpy (buf0, p_block, l * l * sizeof (double));
          if (inverse_block (buf0, D, buf2, l) == INAPPLICABLE)
            return INAPPLICABLE;
          if (norm_block (D, l) < EPS / norm_A)
            return INAPPLICABLE;
        }
      // step 1
      for (int j = s + 1; j < bl; j++)
        {
          int v = s < k ? m : l, h = j < k ? m : l;
          p_block = get_block (A, s, j, n, m);
          mult_block (D, p_block, buf0, v, v, h);
          memcpy (p_block, buf0, v * h * sizeof (double));
        }
      for (int j = 0; j < bl; j++)
        {
          int v = s < k ? m : l, h = j < k ? m : l;
          p_block = get_block (Inv, s, j, n, m);
          mult_block (D, p_block, buf0, v, v, h);
          memcpy (p_block, buf0, v * h * sizeof (double));
        }
      p_block = get_block (A, s, s, n, m);
      int h = s < k ? m : l;
      identity_block (p_block, h);

      // step 2
      for (int i = 0; i < bl; i++)
        if (i != s)
          {
            double *block_is = get_block (A, i, s, n, m);
            for (int j = s + 1; j < bl; j++)
              {
                int v = i < k ? m : l, h = j < k ? m : l, t = s < k ? m : l;
                double *block_ij = get_block (A, i, j, n, m);
                double *block_sj = get_block (A, s, j, n, m);
                diff_mult_block (block_ij, block_is, block_sj, v, t, h);
              }
            for (int j = 0; j < bl; j++)
              {
                int v = i < k ? m : l, h = j < k ? m : l, t = s < k ? m : l;
                double *block_ij = get_block (Inv, i, j, n, m);
                double *block_sj = get_block (Inv, s, j, n, m);
                diff_mult_block (block_ij, block_is, block_sj, v, t, h);
              }

            int v = i < k ? m : l, h = s < k ? m : l;
            zero_block (block_is, v, h);
          }
    }
  return OK;
}

void
identity_matrix (double *X, int n, int m)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  for (int p = 0; p < bl; p++)
    for (int q = 0; q < bl; q++)
      {
        double *p_block = get_block (X, p, q, n, m);
        int v = p < k ? m : l, h = q < k ? m : l; // block size
        if (p != q)
          zero_block (p_block, v, h);
        else
          identity_block (p_block, h);
      }
}

/*void // res -= AxB (r1 = ||AA^-1 - E||_1)
diff_mult (double *res, double *A, double *B, int n, int m)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  for (int i = 0; i < bl; i++)
    for (int j = 0; j < bl; j++)
      {
        int v = i < k ? m : l, h = j < k ? m : l; // block size
        double *p_res = get_block (res, i, j, n, m);
        for (int s = 0; s < bl; s++)
          {
            int t = s < k ? m : l;
            double *pA = get_block (A, i, s, n, m);
            double *pB = get_block (B, s, j, n, m);
            diff_mult_block (p_res, pA, pB, v, t, h);
          }
      }
} */

double
norm (double *A, int n, int m)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  double norm = -1;
  for (int col_ind = 0; col_ind < n; col_ind++)
    {
      double sum = 0;
      int q = col_ind / m;
      int h = q < k ? m : l;
      for (int p = 0; p < bl; p++)
        {
          double *p_block = get_block (A, p, q, n, m);
          int j = col_ind % m;
          int v = p < k ? m : l;
          for (int i = 0; i < v; i++)
            sum += fabs (p_block[i * h + j]);
        }
      if (sum > norm)
        norm = sum;
    }
  return norm;
}

void
swap (double *A, double *buf, int i, int j, int n, int m)
{
  double *p_blockstr_i = get_block (A, i, 0, n, m);
  double *p_blockstr_j = get_block (A, j, 0, n, m);
  memcpy (buf, p_blockstr_i, n * m * sizeof (double));
  memcpy (p_blockstr_i, p_blockstr_j, n * m * sizeof (double));
  memcpy (p_blockstr_j, buf, n * m * sizeof (double));
}

double
residual (double *A, double *Inv, double *str, int n, int m)
{
  int k = n / m, l = n - k * m, bl = l ? k + 1 : k;
  double norm = -1;
  if (n > 11000)
    return 0;
  for (int t = 0; t < n; t++)
    {
      int p = t / m;
      int i = t % m;
      for (int str_ind = 0; str_ind < n; str_ind++)
        {
          int r = str_ind / m;
          int s = str_ind % m;
          int h1 = r < k ? m : l;
          double sum = 0;
          for (int q = 0; q < bl; q++)
            {
              double *p_A = get_block (A, p, q, n, m);
              double *p_Inv = get_block (Inv, q, r, n, m);
              int h = q < k ? m : l;
              for (int j = 0; j < h; j++)
                sum += p_A[i * h + j] * p_Inv[j * h1 + s];
            }
          str[str_ind] = sum;
        }
      str[t] -= 1;
      double cur_norm = 0;
      for (int j = 0; j < n; j++)
        cur_norm += str[j];
      if (cur_norm > norm)
        norm = cur_norm;
    }
  return norm;
}

double *
get_block (double *A, int i, int j, int n, int m)
{
  if (i < n / m)
    return A + i * n * m + j * m * m;
  else
    return A + i * n * m + j * m * (n % m);
}

// single block functions
int
inverse_block (double *block, double *inv, double *buf, int size)
{
  identity_block (inv, size);
  double norm_bl = norm_block (block, size);
  for (int s = 0; s < size; s++)
    {
      // pivot element
      double max_norm = -1;
      int pivot = -1;
      for (int i = s; i < size; i++)
        {
          if (fabs (block[i * size + s]) > max_norm)
            {
              max_norm = fabs (block[i * size + s]);
              pivot = i;
            }
        }
      if (max_norm < EPS * norm_bl)
        return INAPPLICABLE;
      else if (pivot != s)
        {
          swap_block (block, buf, pivot, s, size);
          swap_block (inv, buf, pivot, s, size);
        }

      // step 1
      double d = 1 / block[s * size + s];
      for (int j = s + 1; j < size; j++)
        block[s * size + j] *= d;
      for (int j = 0; j < size; j++)
        inv[s * size + j] *= d;
      block[s * size + s] = 1;

      for (int i = 0; i < size; i++)
        {
          d = block[i * size + s];
          if (i != s)
            {
              for (int j = s + 1; j < size; j++)
                block[i * size + j] -= d * block[s * size + j];
              for (int j = 0; j < size; j++)
                inv[i * size + j] -= d * inv[s * size + j];
              block[i * size + s] = 0;
            }
        }
    }
  return OK;
}

void
identity_block (double *block, int size)
{
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      block[i * size + j] = i != j ? 0 : 1;
}

void
zero_block (double *block, int n, int m)
{
  memset (block, 0, n * m * sizeof (double));
}

void // res = block1 x block2
mult_block (double *block1 /*v x t*/, double *block2 /*t x h*/,
            double *res /*v x h*/, int v, int t, int h)
{
  int v3 = v % 3, h3 = h % 3;
  zero_block (res, v, h);
  int i = 0, j = 0;
  for (i = 0; i < v3; i++)
    {
      for (j = 0; j < h3; j++)
        {
          double sum = 0;
          for (int k = 0; k < t; k++)
            sum += block1[i * t + k] * block2[k * h + j];
          res[i * h + j] += sum;
        }
      for (; j < h; j += 3)
        {
          double s00 = 0, s01 = 0, s02 = 0;
          for (int k = 0; k < t; k++)
            {
              s00 += block1[i * t + k] * block2[k * h + j];
              s01 += block1[i * t + k] * block2[k * h + j + 1];
              s02 += block1[i * t + k] * block2[k * h + j + 2];
            }
          res[i * h + j] += s00;
          res[i * h + j + 1] += s01;
          res[i * h + j + 2] += s02;
        }
    }

  for (; i < v; i += 3)
    {
      for (j = 0; j < h3; j++)
        {
          double s00 = 0, s10 = 0, s20 = 0;
          for (int k = 0; k < t; k++)
            {
              s00 += block1[i * t + k] * block2[k * h + j];
              s10 += block1[(i + 1) * t + k] * block2[k * h + j];
              s20 += block1[(i + 2) * t + k] * block2[k * h + j];
            }
          res[i * h + j] += s00;
          res[(i + 1) * h + j] += s10;
          res[(i + 2) * h + j] += s20;
        }
      for (; j < h; j += 3)
        {
          double s00 = 0, s01 = 0, s02 = 0, s10 = 0, s11 = 0, s12 = 0, s20 = 0,
                 s21 = 0, s22 = 0;
          for (int k = 0; k < t; k++)
            {
              s00 += block1[i * t + k] * block2[k * h + j];
              s01 += block1[i * t + k] * block2[k * h + j + 1];
              s02 += block1[i * t + k] * block2[k * h + j + 2];
              s10 += block1[(i + 1) * t + k] * block2[k * h + j];
              s11 += block1[(i + 1) * t + k] * block2[k * h + j + 1];
              s12 += block1[(i + 1) * t + k] * block2[k * h + j + 2];
              s20 += block1[(i + 2) * t + k] * block2[k * h + j];
              s21 += block1[(i + 2) * t + k] * block2[k * h + j + 1];
              s22 += block1[(i + 2) * t + k] * block2[k * h + j + 2];
            }
          res[i * h + j] += s00;
          res[i * h + j + 1] += s01;
          res[i * h + j + 2] += s02;
          res[(i + 1) * h + j] += s10;
          res[(i + 1) * h + j + 1] += s11;
          res[(i + 1) * h + j + 2] += s12;
          res[(i + 2) * h + j] += s20;
          res[(i + 2) * h + j + 1] += s21;
          res[(i + 2) * h + j + 2] += s22;
        }
    }
}

void // res -= block1 x block2
diff_mult_block (double *res /*v x h*/, double *block1 /*v x t*/,
                 double *block2 /*t x h*/, int v, int t, int h)
{
  int v3 = v % 3, h3 = h % 3;
  int i = 0, j = 0;
  for (; i < v3; i++)
    {
      for (j = 0; j < h3; j++)
        {
          double sum = 0;
          for (int k = 0; k < t; k++)
            sum += block1[i * t + k] * block2[k * h + j];
          res[i * h + j] -= sum;
        }
      for (; j < h; j += 3)
        {
          double s00 = 0, s01 = 0, s02 = 0;
          for (int k = 0; k < t; k++)
            {
              s00 += block1[i * t + k] * block2[k * h + j];
              s01 += block1[i * t + k] * block2[k * h + j + 1];
              s02 += block1[i * t + k] * block2[k * h + j + 2];
            }
          res[i * h + j] -= s00;
          res[i * h + j + 1] -= s01;
          res[i * h + j + 2] -= s02;
        }
    }

  for (; i < v; i += 3)
    {
      for (j = 0; j < h3; j++)
        {
          double s00 = 0, s10 = 0, s20 = 0;
          for (int k = 0; k < t; k++)
            {
              s00 += block1[i * t + k] * block2[k * h + j];
              s10 += block1[(i + 1) * t + k] * block2[k * h + j];
              s20 += block1[(i + 2) * t + k] * block2[k * h + j];
            }
          res[i * h + j] -= s00;
          res[(i + 1) * h + j] -= s10;
          res[(i + 2) * h + j] -= s20;
        }
      for (; j < h; j += 3)
        {
          double s00 = 0, s01 = 0, s02 = 0, s10 = 0, s11 = 0, s12 = 0, s20 = 0,
                 s21 = 0, s22 = 0;
          for (int k = 0; k < t; k++)
            {
              s00 += block1[i * t + k] * block2[k * h + j];
              s01 += block1[i * t + k] * block2[k * h + j + 1];
              s02 += block1[i * t + k] * block2[k * h + j + 2];
              s10 += block1[(i + 1) * t + k] * block2[k * h + j];
              s11 += block1[(i + 1) * t + k] * block2[k * h + j + 1];
              s12 += block1[(i + 1) * t + k] * block2[k * h + j + 2];
              s20 += block1[(i + 2) * t + k] * block2[k * h + j];
              s21 += block1[(i + 2) * t + k] * block2[k * h + j + 1];
              s22 += block1[(i + 2) * t + k] * block2[k * h + j + 2];
            }
          res[i * h + j] -= s00;
          res[i * h + j + 1] -= s01;
          res[i * h + j + 2] -= s02;
          res[(i + 1) * h + j] -= s10;
          res[(i + 1) * h + j + 1] -= s11;
          res[(i + 1) * h + j + 2] -= s12;
          res[(i + 2) * h + j] -= s20;
          res[(i + 2) * h + j + 1] -= s21;
          res[(i + 2) * h + j + 2] -= s22;
        }
    }
}

double
norm_block (double *block, int size)
{
  double norm = 0;
  for (int j = 0; j < size; j++)
    {
      double sum = 0;
      for (int i = 0; i < size; i++)
        sum += fabs (block[i * size + j]);
      if (sum > norm)
        norm = sum;
    }
  return norm;
}

void
swap_block (double *block, double *buf, int i, int j, int size)
{
  memcpy (buf, block + i * size, size * sizeof (double));
  memcpy (block + i * size, block + j * size, size * sizeof (double));
  memcpy (block + j * size, buf, size * sizeof (double));
}