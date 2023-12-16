#include "mmap.h"

#ifdef _WIN32
  #include <windows.h>
#elif __linux
  #include <fcntl.h>
  #include <string.h>
  #include <unistd.h>
#endif  // _WIN32
