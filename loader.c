#include <stdio.h>

#include "common.h"


void print_c (color *c) {
  printf("rgb(%X, %X, %X)\n", c->r, c->g, c->b);
}

int take_c (color *c, FILE *fp) {

  char val[3];
  fgets(val, 4, fp);

  if (feof(fp)) {
    // Not enough characters left
    return 0;
  }

  c->r = val[0];
  c->g = val[1];
  c->b = val[2];

  return 1;
}


blocks load (char *name) {
  FILE *fp = fopen(name, "r");
  if (fp == NULL) {
    printf("Error: Cannot open file: '%s'\n", name);
    return;
  }

  color c;
  while (take_c(&c, fp)) {
    print_c(&c);
  }

  fclose(fp);
}


int main (int argc, char *argv) {

  blocks scene = load("test");

  return 0;
}
