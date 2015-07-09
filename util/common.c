#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
