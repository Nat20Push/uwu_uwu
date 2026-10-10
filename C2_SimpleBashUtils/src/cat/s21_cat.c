#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef struct FlagGroup {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} CatOptions;

typedef struct ReaderState {
  unsigned long line_number;
  int line_start;
  int previous_line_was_empty;
} ReaderState;

static void PrintUsage(const char* program_name) {
  fprintf(stderr, "Usage: %s [OPTION]... [FILE]...\n", program_name);
}

static int ParseLongOption(const char* argument, CatOptions* options) {
  int valid = 1;

  if (strcmp(argument, "--number-nonblank") == 0) {
    options->b = 1;
  } else if (strcmp(argument, "--number") == 0) {
    options->n = 1;
  } else if (strcmp(argument, "--squeeze-blank") == 0) {
    options->s = 1;
  } else {
    valid = 0;
  }

  return valid;
}

static int ParseShortOptions(const char* argument, CatOptions* options) {
  int valid = 1;

  for (int i = 1; argument[i] != '\0' && valid; i++) {
    switch (argument[i]) {
      case 'b':
        options->b = 1;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 'E':
        options->e = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'T':
        options->t = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      default:
        valid = 0;
        break;
    }
  }

  return valid;
}

static int ParseArguments(int argc, char* argv[], CatOptions* options,
                          char* files[], int* file_count) {
  int valid = 1;
  int options_finished = 0;

  for (int i = 1; i < argc && valid; i++) {
    if (!options_finished && strcmp(argv[i], "--") == 0) {
      options_finished = 1;
    } else if (!options_finished && argv[i][0] == '-' && argv[i][1] != '\0') {
      if (argv[i][1] == '-') {
        valid = ParseLongOption(argv[i], options);
      } else {
        valid = ParseShortOptions(argv[i], options);
      }

      if (!valid) {
        fprintf(stderr, "%s: invalid option -- '%s'\n", argv[0], argv[i]);
        PrintUsage(argv[0]);
      }
    } else {
      files[*file_count] = argv[i];
      (*file_count)++;
    }
  }

  return valid;
}

static void PrintLineNumber(unsigned long line_number) {
  printf("%6lu\t", line_number);
}

static void PrintVisibleCharacter(unsigned char character,
                                  const CatOptions* options) {
  if (character == '\t' && options->t) {
    fputs("^I", stdout);
  } else if (options->v && character != '\n' && character != '\t') {
    if (character >= 128) {
      fputs("M-", stdout);
      character -= 128;
    }

    if (character < 32) {
      putchar('^');
      putchar(character + 64);
    } else if (character == 127) {
      fputs("^?", stdout);
    } else {
      putchar(character);
    }
  } else {
    putchar(character);
  }
}

static int ReadStream(FILE* stream, const CatOptions* options,
                      ReaderState* state) {
  int character;

  while ((character = fgetc(stream)) != EOF) {
    if (state->line_start) {
      int current_line_is_empty = character == '\n';

      if (options->s && current_line_is_empty &&
          state->previous_line_was_empty) {
        continue;
      }

      if ((options->b && !current_line_is_empty) ||
          (options->n && !options->b)) {
        PrintLineNumber(state->line_number);
        state->line_number++;
      }

      state->previous_line_was_empty = current_line_is_empty;
      state->line_start = 0;
    }

    if (character == '\r' && options->e && !options->v) {
      int next_character = fgetc(stream);
      if (next_character == '\n') {
        fputs("^M", stdout);
        character = '\n';
      } else {
        putchar('\r');
        if (next_character != EOF) {
          ungetc(next_character, stream);
        }
        continue;
      }
    }

    if (character == '\n') {
      if (options->e) {
        putchar('$');
      }
      putchar('\n');
      state->line_start = 1;
    } else {
      PrintVisibleCharacter((unsigned char)character, options);
    }
  }

  return ferror(stream) == 0;
}

static int ReadFile(const char* file_name, const CatOptions* options,
                    ReaderState* state, const char* program_name) {
  int success = 1;
  FILE* file = fopen(file_name, "rb");

  if (file == NULL) {
    fprintf(stderr, "%s: %s: %s\n", program_name, file_name, strerror(errno));
    success = 0;
  } else {
    if (!ReadStream(file, options, state)) {
      fprintf(stderr, "%s: %s: %s\n", program_name, file_name, strerror(errno));
      success = 0;
    }
    fclose(file);
  }

  return success;
}

int main(int argc, char* argv[]) {
  CatOptions options = {0};
  ReaderState state = {1, 1, 0};
  char* files[argc];
  int file_count = 0;
  int exit_code = 0;

  if (!ParseArguments(argc, argv, &options, files, &file_count)) {
    exit_code = 1;
  } else if (file_count == 0) {
    if (!ReadStream(stdin, &options, &state)) {
      fprintf(stderr, "%s: stdin: %s\n", argv[0], strerror(errno));
      exit_code = 1;
    }
  } else {
    for (int i = 0; i < file_count; i++) {
      if (strcmp(files[i], "-") == 0) {
        if (!ReadStream(stdin, &options, &state)) {
          fprintf(stderr, "%s: stdin: %s\n", argv[0], strerror(errno));
          exit_code = 1;
        }
      } else if (!ReadFile(files[i], &options, &state, argv[0])) {
        exit_code = 1;
      }
    }
  }

  return exit_code;
}
