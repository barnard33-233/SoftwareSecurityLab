#include <string.h>
#include <stdlib.h>
#include "input.h"

struct Filename cutFilename(char * filename_ref){
  char delime = '.';
  int filename_len = strlen(filename_ref);
  char * filename = malloc(filename_len);
  strcpy(filename, filename_ref);

  char * ext;
  char * name;
  
  for(int i = filename_len - 1; i >= 0; i --){
    if(filename[i] == delime){
      ext = filename + i + 1;
      name = filename;
      filename[i] = '\0';
    }
  }

  struct Filename fn;
  fn.ext = ext;
  fn.name = name;
  return fn;
}

inline void stripeSpace(char* c, int len){
  for(int i = len - 1; i >= 0; i--){
    if(c[i] != '\0' && c[i] != ' '){
      break;
    }
    c[i] = '\0';
  }
}
