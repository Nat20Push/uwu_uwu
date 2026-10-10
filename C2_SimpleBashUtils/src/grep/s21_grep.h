#ifndef S21_GREP_H_
#define S21_GREP_H_

#include <regex.h>
#include <stddef.h>

typedef struct GrepOptions {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} GrepOptions;

typedef struct StringList {
  char **items;
  size_t count;
  size_t capacity;
} StringList;

typedef struct RegexList {
  regex_t *items;
  size_t count;
} RegexList;

typedef struct FileResult {
  int selected_lines;
  int error;
} FileResult;

#endif  // S21_GREP_H_
