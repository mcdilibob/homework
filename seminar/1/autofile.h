#include <cstdio>

class autofile
{
private:
  FILE *fp = nullptr;

public:
  autofile () = default;
  autofile (const char *filename, const char *mode) { fp = fopen (filename, mode); }
  ~autofile ()
  {
    if (fp)
      fclose (fp);
  }
  FILE *
  get ()
  {
    return fp;
  }
  bool
  is_open ()
  {
    return fp != nullptr;
  }
  bool
  is_end ()
  {
    return feof (fp);
  }
  bool
  is_error ()
  {
    return ferror (fp);
  }
};