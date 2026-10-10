#!/bin/sh

set -u

PROGRAM=./s21_grep
SYSTEM_GREP=$(command -v grep)
TEST_DIR=$(mktemp -d)
INPUT=$TEST_DIR/input.txt
PATTERNS=$TEST_DIR/patterns.txt
EXPECTED=$TEST_DIR/expected.txt
ACTUAL=$TEST_DIR/actual.txt
PASSED=0
FAILED=0

trap 'rm -f "$INPUT" "$PATTERNS" "$EXPECTED" "$ACTUAL"; rmdir "$TEST_DIR"' EXIT

cat > "$INPUT" <<'EOF'
Alpha apple 123
beta BANANA 456
Gamma apple apple 789
error: something failed
WARNING: low disk

email test@example.com
cat scatter concatenate
AaAa
EOF
printf 'last line without newline' >> "$INPUT"

cat > "$PATTERNS" <<'EOF'
apple
error
[0-9][0-9]*
EOF

run_test() {
  name=$1
  shift

  "$SYSTEM_GREP" "$@" > "$EXPECTED" 2>/dev/null
  expected_status=$?
  "$PROGRAM" "$@" > "$ACTUAL" 2>/dev/null
  actual_status=$?

  if cmp -s "$EXPECTED" "$ACTUAL" &&
    [ "$expected_status" -eq "$actual_status" ]; then
    echo "PASS  $name"
    PASSED=$((PASSED + 1))
  else
    echo "FAIL  $name"
    diff -u "$EXPECTED" "$ACTUAL" || true
    echo "exit: expected $expected_status, actual $actual_status"
    FAILED=$((FAILED + 1))
  fi
}

run_test "обычный поиск" apple "$INPUT"
run_test "-e" -e apple "$INPUT"
run_test "несколько -e" -e apple -e error "$INPUT"
run_test "-i" -i alpha "$INPUT"
run_test "-v" -v apple "$INPUT"
run_test "-c" -c apple "$INPUT"
run_test "-l" -l apple "$INPUT"
run_test "-n" -n apple "$INPUT"
run_test "-h" -h apple "$INPUT" "$INPUT"
run_test "-s" -s apple missing.txt "$INPUT"
run_test "-f" -f "$PATTERNS" "$INPUT"
run_test "-o" -o apple "$INPUT"
run_test "-on" -on '[0-9][0-9]*' "$INPUT"
run_test "-iv" -iv apple "$INPUT"
run_test "-ic" -ic alpha "$INPUT"
run_test "-in" -in alpha "$INPUT"
run_test "-cv" -cv apple "$INPUT"

# BSD grep в macOS при -cl печатает счетчик и имя.
# GNU/BusyBox дают приоритет -l и печатают только имя файла.
printf '%s\n' "$INPUT" > "$EXPECTED"
"$PROGRAM" -cl apple "$INPUT" > "$ACTUAL" 2>/dev/null
actual_status=$?
if cmp -s "$EXPECTED" "$ACTUAL" && [ "$actual_status" -eq 0 ]; then
  echo "PASS  -cl"
  PASSED=$((PASSED + 1))
else
  echo "FAIL  -cl"
  diff -u "$EXPECTED" "$ACTUAL" || true
  FAILED=$((FAILED + 1))
fi

run_test "-nf" -n -f "$PATTERNS" "$INPUT"
run_test "-e и -f" -e warning -f "$PATTERNS" "$INPUT"

echo
echo "RESULT: $PASSED passed, $FAILED failed"

[ "$FAILED" -eq 0 ]
