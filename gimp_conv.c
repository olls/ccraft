#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/common.h"


int get_quote (FILE *fp, char **content) {
  char c = 0;
  int n = 0;
  fpos_t pos;

  // Save position
  fgetpos(fp, &pos);

  while (c != '\"') {
    c = fgetc(fp);
    n++;
    if (c == EOF) {
      error("Cannot match brackets");
      return -1;
    }
  }

  // Move back to start
  fsetpos(fp, &pos);

  // Offset to just before "
  n--;

  *content = (char *)malloc(sizeof(char) * (n + 1)); // Extra for \0

  fread(*content, 1, n, fp);
  (*content)[n] = '\0';

  // Move fp to after "
  fgetc(fp);

  return n;
}

int main(int argc, char *argv[]) {

  FILE *fp = fopen(argv[1], "r");

  if (fp == NULL) {
    error("Cannot open file");
    return 1;
  }

  int n;

  // Move to beginning of char array
  char data[13];
  while (strcmp(data, "*header_data")) {
    n = fscanf(fp, "%12s", data);
  }
  printf("Found: '%s'\n", data);

  n = get_quote(fp);
  printf("%d\n", n);
  n = get_quote(fp);
  printf("%d\n", n);

  fclose(fp);
  return 0;
}
