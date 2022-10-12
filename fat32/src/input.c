#include <string.h>
#include <stdlib.h>
#include "input.h"
#include "type.h"

struct Filename16 cutFilename16(uint16 * filename, int filename_len){
  uint16 delime = '.';
  uint16 * ext;
  uint16 * name;
  
  struct Filename16 fn;
  
  for(int i = filename_len - 1; i >= 0; i --){
    if(filename[i] == delime){
      ext = filename + i + 1;
      name = filename;
      filename[i] = 0;

      fn.ext = ext;
      fn.name = name;
      fn.ext_len = filename_len - i - 1;
      fn.name_len = i;
      return fn;
    }
  }

  fn.name_len = filename_len;
  fn.name = filename;
  fn.ext = NULL;
  fn.ext_len = 0;

  return fn;
}

struct Filename cutFilename(char * filename){
  char delime = '.';
  char * ext;
  char * name;
  int filename_len = strlen(filename);
  
  struct Filename fn;
  
  for(int i = filename_len - 1; i >= 0; i --){
    if(filename[i] == delime){
      ext = filename + i + 1;
      name = filename;
      filename[i] = 0;

      break;
    }
  }

  fn.ext = ext;
  fn.name = name;
  return fn;
}

void stripeSpace(char* c, int len){
  for(int i = len - 1; i >= 0; i--){
    if(c[i] != '\0' && c[i] != ' '){
      break;
    }
    c[i] = '\0';
  }
}

void Uppercase(char* c, int len){
  for(int i = 0; i < len; i++){
    if(c[i] >= 'a' && c[i] <= 'z'){
      c[i] += 'A' - 'a';
    }
  }
}


uint16 * char2wchar(char * s, int len){
  uint16 * d = malloc(sizeof(uint16) * len);
  for(int i = 0; i < len; i++){
    d[i] = (uint16)s[i];
  }
  return d;
}

int wcharncmp(uint16 * s1, uint16 * s2, int n){
  for(int i = 0; i < n; i++){
    if(s1[i] != s2[i]){
      return -1;
    }
  }
  return 0;
}