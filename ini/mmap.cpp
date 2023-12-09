#include "mmap.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#endif  // _WIN32
