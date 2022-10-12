#ifndef _INPUT_H
#define _INPUT_H

#include <string.h>
#include "type.h"


struct Filename{
  char * name;
  char * ext;
};

struct Filename16{
  uint16 * name;
  uint16 * ext;
  int name_len;
  int ext_len;
};

struct Filename cutFilename(char * filename);
struct Filename16 cutFilename16(uint16 * filename, int len);

void stripeSpace(char* c, int len);

void Uppercase(char* c, int len);

uint16 * char2wchar(char * s, int len);

int wcharncmp(uint16 * s1, uint16 * s2, int n);

#endif