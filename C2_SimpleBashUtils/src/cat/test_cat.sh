#!/bin/sh

set -u

PROGRAM=./s21_cat
SYSTEM_CAT=$(command -v cat)
TEST_DIR=$(mktemp -d) || exit 1
INPUT=$TEST_DIR/input.txt
SECOND=$TEST_DIR/second.txt
EMPTY=$TEST_DIR/empty.txt
EXPECTED=$TEST_DIR/expected.txt
ACTUAL=$TEST_DIR/actual.txt
PASSED=0
FAILED=0
SKIPPED=0

trap 'rm -f "$INPUT" "$SECOND" "$EMPTY" "$EXPECTED" "$ACTUAL"; rmdir "$TEST_DIR"' EXIT

printf 'first\n\n\n\tsecond\r\ncontrol: \001\177\nlast' > "$INPUT"
printf 'second file\n\n' > "$SECOND"
: > "$EMPTY"

run_test() {
  name=$1
  shift

  "$SYSTEM_CAT" "$@" > "$EXPECTED" 2>/dev/null
  expected_status=$?
  "$PROGRAM" "$@" > "$ACTUAL" 2>/dev/null
  actual_status=$?

  if cmp -s "$EXPECTED" "$ACTUAL" && [ "$expected_status" -eq "$actual_status" ]; then
    echo "PASS  $name"
    PASSED=$((PASSED + 1))
  else
    echo "FAIL  $name"
    diff -u "$EXPECTED" "$ACTUAL" || true
    echo "exit: expected $expected_status, actual $actual_status"
    FAILED=$((FAILED + 1))
  fi
}

run_stdin_test() {
  name=$1
  shift

  "$SYSTEM_CAT" "$@" < "$INPUT" > "$EXPECTED" 2>/dev/null
  expected_status=$?
  "$PROGRAM" "$@" < "$INPUT" > "$ACTUAL" 2>/dev/null
  actual_status=$?

  if cmp -s "$EXPECTED" "$ACTUAL" && [ "$expected_status" -eq "$actual_status" ]; then
    echo "PASS  $name"
    PASSED=$((PASSED + 1))
  else
    echo "FAIL  $name"
    diff -u "$EXPECTED" "$ACTUAL" || true
    echo "exit: expected $expected_status, actual $actual_status"
    FAILED=$((FAILED + 1))
  fi
}

run_if_supported() {
  name=$1
  option=$2
  if "$SYSTEM_CAT" "$option" /dev/null >/dev/null 2>&1; then
    run_test "$name" "$option" "$INPUT"
  else
    echo "SKIP  $name (системный cat не поддерживает $option)"
    SKIPPED=$((SKIPPED + 1))
  fi
}

run_test "без флагов" "$INPUT"
run_test "пустой файл" "$EMPTY"
run_test "несколько файлов" "$INPUT" "$SECOND"
run_test "-b" -b "$INPUT"
run_test "-e" -e "$INPUT"
run_test "-n" -n "$INPUT"
run_test "-s" -s "$INPUT"
run_test "-t" -t "$INPUT"
run_test "-v" -v "$INPUT"
run_test "-b и -n" -bn "$INPUT"
run_test "-s между файлами" -s "$INPUT" "$SECOND"
run_stdin_test "stdin без аргументов"
run_stdin_test "stdin через -" -n -
run_if_supported "-E" -E
run_if_supported "-T" -T
run_if_supported "--number-nonblank" --number-nonblank
run_if_supported "--number" --number
run_if_supported "--squeeze-blank" --squeeze-blank

echo
echo "RESULT: $PASSED passed, $FAILED failed, $SKIPPED skipped"

[ "$FAILED" -eq 0 ]
