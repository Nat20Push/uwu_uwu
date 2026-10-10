#define _POSIX_C_SOURCE 200809L

#include "s21_grep.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void PrintUsage(const char *program_name) {
  fprintf(stderr,
          "Usage: %s [-eivclnhsfo] PATTERN [FILE...]\n"
          "       %s [-ivclnhso] -e PATTERN... [-f FILE]... [FILE...]\n",
          program_name, program_name);
}

static int AddString(StringList *list, const char *text) {
  int success = 1;

  if (list->count == list->capacity) {
    size_t new_capacity = list->capacity == 0 ? 4 : list->capacity * 2;
    char **new_items = realloc(list->items, new_capacity * sizeof(char *));

    if (new_items == NULL) {
      success = 0;
    } else {
      list->items = new_items;
      list->capacity = new_capacity;
    }
  }

  if (success) {
    size_t length = strlen(text);
    char *copy = malloc(length + 1);

    if (copy == NULL) {
      success = 0;
    } else {
      memcpy(copy, text, length + 1);
      list->items[list->count] = copy;
      list->count++;
    }
  }

  return success;
}

static void FreeStringList(StringList *list) {
  for (size_t i = 0; i < list->count; i++) {
    free(list->items[i]);
  }
  free(list->items);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

static int ParseArguments(int argc, char *argv[], GrepOptions *options,
                          StringList *patterns, StringList *pattern_files,
                          int *first_file_index) {
  int success = 1;
  int option;
  int pattern_was_explicit = 0;

  opterr = 0;
  while (success && (option = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (option) {
      case 'e':
        options->e = 1;
        pattern_was_explicit = 1;
        success = AddString(patterns, optarg);
        break;
      case 'i':
        options->i = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'c':
        options->c = 1;
        break;
      case 'l':
        options->l = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 'h':
        options->h = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 'f':
        options->f = 1;
        pattern_was_explicit = 1;
        success = AddString(pattern_files, optarg);
        break;
      case 'o':
        options->o = 1;
        break;
      default:
        fprintf(stderr, "%s: invalid option\n", argv[0]);
        PrintUsage(argv[0]);
        success = 0;
        break;
    }
  }

  if (success && !pattern_was_explicit) {
    if (optind >= argc) {
      PrintUsage(argv[0]);
      success = 0;
    } else {
      success = AddString(patterns, argv[optind]);
      optind++;
    }
  }

  if (!success && errno == ENOMEM) {
    fprintf(stderr, "%s: not enough memory\n", argv[0]);
  }

  *first_file_index = optind;
  return success;
}

static int LoadPatternFile(const char *file_name, const GrepOptions *options,
                           StringList *patterns, const char *program_name) {
  int success = 1;
  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    if (!options->s) {
      fprintf(stderr, "%s: %s: %s\n", program_name, file_name, strerror(errno));
    }
    success = 0;
  } else {
    char *line = NULL;
    size_t capacity = 0;
    ssize_t length;

    while (success && (length = getline(&line, &capacity, file)) != -1) {
      if (length > 0 && line[length - 1] == '\n') {
        line[length - 1] = '\0';
      }
      success = AddString(patterns, line);
    }

    if (ferror(file)) {
      if (!options->s) {
        fprintf(stderr, "%s: %s: %s\n", program_name, file_name,
                strerror(errno));
      }
      success = 0;
    }

    free(line);
    fclose(file);
  }

  return success;
}

static int LoadPatternFiles(const StringList *pattern_files,
                            const GrepOptions *options, StringList *patterns,
                            const char *program_name) {
  int success = 1;

  for (size_t i = 0; i < pattern_files->count; i++) {
    if (!LoadPatternFile(pattern_files->items[i], options, patterns,
                         program_name)) {
      success = 0;
    }
  }

  return success;
}

static int CompilePatterns(const StringList *patterns,
                           const GrepOptions *options, RegexList *regexes,
                           const char *program_name) {
  int success = 1;
  int compile_flags = options->i ? REG_ICASE : 0;

  if (patterns->count > 0) {
    regexes->items = calloc(patterns->count, sizeof(regex_t));
    if (regexes->items == NULL) {
      fprintf(stderr, "%s: not enough memory\n", program_name);
      success = 0;
    }
  }

  for (size_t i = 0; i < patterns->count && success; i++) {
    const char *pattern =
        patterns->items[i][0] == '\0' ? "^" : patterns->items[i];
    int error_code = regcomp(&regexes->items[i], pattern, compile_flags);
    if (error_code != 0) {
      char error_message[256];
      regerror(error_code, &regexes->items[i], error_message,
               sizeof(error_message));
      fprintf(stderr, "%s: %s\n", program_name, error_message);
      success = 0;
    } else {
      regexes->count++;
    }
  }

  return success;
}

static void FreeRegexList(RegexList *regexes) {
  for (size_t i = 0; i < regexes->count; i++) {
    regfree(&regexes->items[i]);
  }
  free(regexes->items);
  regexes->items = NULL;
  regexes->count = 0;
}

static int LineMatches(const char *line, const RegexList *regexes) {
  int matches = 0;

  for (size_t i = 0; i < regexes->count && !matches; i++) {
    matches = regexec(&regexes->items[i], line, 0, NULL, 0) == 0;
  }

  return matches;
}

static void PrintPrefix(const char *display_name, int multiple_files,
                        unsigned long line_number, const GrepOptions *options) {
  if (multiple_files && !options->h) {
    printf("%s:", display_name);
  }
  if (options->n) {
    printf("%lu:", line_number);
  }
}

static void PrintOnlyMatches(const char *line, size_t length,
                             const RegexList *regexes, const char *display_name,
                             int multiple_files, unsigned long line_number,
                             const GrepOptions *options) {
  size_t offset = 0;

  while (offset <= length) {
    size_t best_start = (size_t)-1;
    size_t best_end = 0;

    for (size_t i = 0; i < regexes->count; i++) {
      regmatch_t match;
      int flags = offset > 0 ? REG_NOTBOL : 0;

      if (regexec(&regexes->items[i], line + offset, 1, &match, flags) == 0 &&
          match.rm_so >= 0 && match.rm_eo >= 0) {
        size_t start = offset + (size_t)match.rm_so;
        size_t end = offset + (size_t)match.rm_eo;

        if (start < best_start || (start == best_start && end > best_end)) {
          best_start = start;
          best_end = end;
        }
      }
    }

    if (best_start == (size_t)-1) {
      break;
    }

    if (best_end == best_start) {
      if (best_start < length) {
        offset = best_start + 1;
      } else {
        break;
      }
    } else {
      PrintPrefix(display_name, multiple_files, line_number, options);
      fwrite(line + best_start, 1, best_end - best_start, stdout);
      putchar('\n');
      offset = best_end;
    }
  }
}

static FileResult ProcessStream(FILE *stream, const char *display_name,
                                int multiple_files, const GrepOptions *options,
                                const RegexList *regexes) {
  FileResult result = {0, 0};
  char *line = NULL;
  size_t capacity = 0;
  ssize_t read_length;
  unsigned long line_number = 0;

  while ((read_length = getline(&line, &capacity, stream)) != -1) {
    line_number++;
    size_t content_length = (size_t)read_length;

    if (content_length > 0 && line[content_length - 1] == '\n') {
      content_length--;
      line[content_length] = '\0';
    }

    int matches = LineMatches(line, regexes);
    int selected = options->v ? !matches : matches;

    if (selected) {
      result.selected_lines++;

      if (options->l) {
        printf("%s\n", display_name);
        break;
      }

      if (!options->c) {
        if (options->o) {
          if (!options->v) {
            PrintOnlyMatches(line, content_length, regexes, display_name,
                             multiple_files, line_number, options);
          }
        } else {
          PrintPrefix(display_name, multiple_files, line_number, options);
          fwrite(line, 1, content_length, stdout);
          putchar('\n');
        }
      }
    }
  }

  if (ferror(stream)) {
    result.error = 1;
  }

  if (options->c && !options->l) {
    if (multiple_files && !options->h) {
      printf("%s:", display_name);
    }
    printf("%d\n", result.selected_lines);
  }

  free(line);
  return result;
}

static FileResult ProcessFile(const char *file_name, int multiple_files,
                              const GrepOptions *options,
                              const RegexList *regexes,
                              const char *program_name) {
  FileResult result = {0, 0};
  FILE *file = NULL;
  const char *display_name = file_name;

  if (strcmp(file_name, "-") == 0) {
    file = stdin;
    display_name = "(standard input)";
  } else {
    file = fopen(file_name, "r");
  }

  if (file == NULL) {
    if (!options->s) {
      fprintf(stderr, "%s: %s: %s\n", program_name, file_name, strerror(errno));
    }
    result.error = 1;
  } else {
    result =
        ProcessStream(file, display_name, multiple_files, options, regexes);
    if (result.error && !options->s) {
      fprintf(stderr, "%s: %s: %s\n", program_name, file_name, strerror(errno));
    }
    if (file != stdin) {
      fclose(file);
    }
  }

  return result;
}

int main(int argc, char *argv[]) {
  GrepOptions options = {0};
  StringList patterns = {0};
  StringList pattern_files = {0};
  RegexList regexes = {0};
  int first_file_index = 0;
  int exit_code = 2;

  int arguments_valid = ParseArguments(argc, argv, &options, &patterns,
                                       &pattern_files, &first_file_index);
  int pattern_files_loaded = 0;
  int patterns_compiled = 0;

  if (arguments_valid) {
    pattern_files_loaded =
        LoadPatternFiles(&pattern_files, &options, &patterns, argv[0]);
  }
  if (arguments_valid && pattern_files_loaded) {
    patterns_compiled = CompilePatterns(&patterns, &options, &regexes, argv[0]);
  }

  if (arguments_valid && pattern_files_loaded && patterns_compiled) {
    int file_count = argc - first_file_index;
    int multiple_files = file_count > 1;
    int any_selected = 0;
    int any_error = 0;

    if (file_count == 0) {
      FileResult result =
          ProcessStream(stdin, "(standard input)", 0, &options, &regexes);
      any_selected = result.selected_lines > 0;
      any_error = result.error;
    } else {
      for (int i = first_file_index; i < argc; i++) {
        FileResult result =
            ProcessFile(argv[i], multiple_files, &options, &regexes, argv[0]);
        if (result.selected_lines > 0) {
          any_selected = 1;
        }
        if (result.error) {
          any_error = 1;
        }
      }
    }

    if (any_error) {
      exit_code = 2;
    } else {
      exit_code = any_selected ? 0 : 1;
    }
  }

  FreeRegexList(&regexes);
  FreeStringList(&pattern_files);
  FreeStringList(&patterns);
  return exit_code;
}
