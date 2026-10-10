CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g

s21_grep: s21_grep.c s21_grep.h
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep

.PHONY: test clean rebuild

test: s21_grep
	./test_grep.sh

clean:
	rm -f s21_grep

rebuild: clean s21_grep
