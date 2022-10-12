#ifndef _INPUT_H
#define _INPUT_H

#include <string.h>

struct Filename{
  char * name;
  char * ext;
};

struct Filename cutFilename(char * filename_ref);

inline void stripeSpace(char* c, int len);

#endif