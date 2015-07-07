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


char *unslash_slashes (char *str) {

  size_t len = strlen(str);
  char *new = (char *)malloc(sizeof(char) * len);

  char *new_pt = new;
  char *old_pt = str;

  char prev = 0;

  while (*old_pt != 0) {
    if (!(*old_pt == '\\' && prev == '\\')) {
      *new_pt = *old_pt;
      new_pt++;
    }

    prev = *old_pt++;
  }
  *new_pt = '\0';

  return new;
}


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

  // Cut out '_gimp' from filename
  int len = strlen(argv[1]);
  int g_len = strlen("_gimp");
  char *new_filename = (char *)malloc(sizeof(argv[1]) * (len - g_len));
  strcpy(new_filename, argv[1]);
  strcpy(new_filename + len - g_len - 2, ".h\0");

  printf("Outputting to '%s'\n", new_filename);

  // Write result
  FILE *fp_ = fopen(new_filename, "w");
  if (fp_ == NULL) {
    error("Cannot open file");
    return 1;
  }

  fprintf(fp_, "static char *block = \"%s\";\n", data);

  if (part != NULL) {
    free(part);
  }

  return 0;
}