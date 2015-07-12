#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>


void
error(char * msg, ...)
{
  char buf[256];

  va_list aptr;
  va_start(aptr, msg);
  vsnprintf(buf, 256, msg, aptr);
  va_end(aptr);

  fprintf(stderr, "\e[01;31m%s\e[0m\n", buf);
}


uint64_t
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * (uint64_t)1000000) + (uint64_t)tv.tv_usec;
}
