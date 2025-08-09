#!/usr/bin/env bash
# build.sh – replacement for the original Makefile
set -euo pipefail

#######################################
# Configuration (mirrors the Makefile)
#######################################
SOURCENAME="asm8086.c"
EXECNAME="${SOURCENAME%.*}" # asm8086

DEBUGEXTENSION=".dSYM"
DEBUGNAME="${EXECNAME}${DEBUGEXTENSION}"

TESTFILENAME="test_minimal.asm"
BASEFILENAME="${TESTFILENAME%.*}"
BINFILENAME="${BASEFILENAME}.bin"
NASMFILENAME="${BASEFILENAME}_nasm.bin"
PPFILENAME="${EXECNAME}.i"              # pre-processor output
OUT_DIR="build"

CC="${CC:-clang}"                       # allow CC=gcc ./build.sh …

CFLAGS=(
  -Wall -Wextra -Wpedantic -Wconversion
  -Wformat=2 -Wno-unused-parameter -Wshadow -Wvla
  -Wwrite-strings -Wstrict-prototypes -Wold-style-definition
  -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
  -Wno-gnu-binary-literal -Wno-c11-extensions
  -O0 -std=c99
)

# Extra warnings for GCC
if [[ $CC == gcc ]]; then
  CFLAGS+=( -Wjump-misses-init -Wlogical-op )
fi

INCLUDE="-I."

#######################################
# Target functions
#######################################
build() {
  echo "==> Building ${EXECNAME}"
  "$CC" "${CFLAGS[@]}" $INCLUDE -g -o "$OUT_DIR/$EXECNAME" "$SOURCENAME"
}

buildnew() {
    echo "==> Compiling just tables for testing"
    "$CC" "${CFLAGS[@]}" "-E" $INCLUDE -g -o "$OUT_DIR/asm8086new" "./asm8086new.c"
    echo "   ✔️  Saved to asm8086new"
}


run() {
  build
  echo "==> Running ${EXECNAME}"
  "./$EXECNAME" "$TESTFILENAME"
}

test() {
  build
  echo "==> Assembling test file with NASM"
  nasm "$TESTFILENAME" -o "$NASMFILENAME"
  echo "==> Diffing binaries"
  diff "$BINFILENAME" "$NASMFILENAME"
}

buildpp() {
  echo "==> Generating preprocessor output (${PPFILENAME})"
  "$CC" "${CFLAGS[@]}" $INCLUDE -E "$SOURCENAME" > "$PPFILENAME"
  echo "   ✔️  Saved to $PPFILENAME"
}

dummytest() {
    echo "==> Compiling dummy test"
    "$CC" "${CFLAGS[@]}" $INCLUDE -g -o "$OUT_DIR/dummy_test" "./tests/dummy_test.c"
    echo "   ✔️  Saved to dummy_test"
}

teststring() {
    echo "==> Compiling tests_tring"
    "$CC" "${CFLAGS[@]}" $INCLUDE -g -o "$OUT_DIR/test_string" "./tests/test_string.c"
    echo "   ✔️  Saved to test_string"
}

clean() {
  echo "==> Cleaning generated files"
  rm -rf "$NASMFILENAME" "$BINFILENAME" "$EXECNAME" "$DEBUGNAME" "$PPFILENAME"
}

#######################################
# Dispatch
#######################################
case "${1:-build}" in
  build)       build      ;;
  buildnew)    buildnew   ;;
  run)         run        ;;
  test)        test       ;;
  buildpp)     buildpp    ;;
  dummytest)   dummytest  ;;
  test_string) teststring ;;
  clean)       clean      ;;
  *)
    echo "Usage: $0 [build|run|test|pp|dummytest|clean]"
    exit 1
    ;;
esac
