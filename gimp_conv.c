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


  FILE *fp = fopen(argv[1], "r");

int main (int argc, char *argv[]) {

  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
    error("Cannot open file");
    return 1;
  }

  // Move to beginning of char array
  char word[13];
  while (strcmp(word, "*header_data")) {
    if (fscanf(fp, "%12s", word) <= 0) {
      error("Cannot find data");
      return 1;
    }
  }

  char *part;
  char data[256];
  int n = 0,
      i = 0;
  while (n != -1) {
    // Move to first quote
    get_quote(fp, &part);

    n = get_quote(fp, &part);
    if (n > 0) {
      strcpy(data + (i * 64), unslash_slashes(part));
    }

    i++;
  }

  fclose(fp);
  return 0;
}
