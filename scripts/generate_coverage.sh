#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 2 ]]; then
    printf 'Usage: %s <build-dir> <source-dir>\n' "$0" >&2
    exit 1
fi

BUILD_DIR=$1
SOURCE_DIR=$2
REPO_NAME=$(basename "$SOURCE_DIR")
TEST_BINARY="$BUILD_DIR/bin/cslex_gtests"
COVERAGE_DIR="$BUILD_DIR/coverage"
PROFILE_DIR="$COVERAGE_DIR/profiles"
PROFDATA="$COVERAGE_DIR/cslex.profdata"
SUMMARY_TXT="$COVERAGE_DIR/summary.txt"
SUMMARY_JSON="$COVERAGE_DIR/summary.json"
HTML_DIR="$COVERAGE_DIR/html"
IGNORE_REGEX="(^tests/.*|.*/${REPO_NAME}/tests/.*|.*/_deps/.*|.*/CMakeFiles/.*|/Library/Developer/CommandLineTools/.*|/Applications/Xcode.app/.*|/usr/include/.*)"

if [[ ! -x "$TEST_BINARY" ]]; then
    printf 'Coverage binary not found: %s\n' "$TEST_BINARY" >&2
    exit 1
fi

mkdir -p "$PROFILE_DIR"
rm -f "$PROFILE_DIR"/*.profraw
rm -f "$PROFDATA" "$SUMMARY_TXT" "$SUMMARY_JSON"
rm -rf "$HTML_DIR"

printf 'Running tests with coverage instrumentation...\n'
LLVM_PROFILE_FILE="$PROFILE_DIR/cslex-%p.profraw" ctest --test-dir "$BUILD_DIR" --output-on-failure

shopt -s nullglob
profraw_files=("$PROFILE_DIR"/*.profraw)
shopt -u nullglob

if [[ ${#profraw_files[@]} -eq 0 ]]; then
    printf 'No .profraw files were generated in %s\n' "$PROFILE_DIR" >&2
    exit 1
fi

printf 'Merging raw profiles...\n'
xcrun llvm-profdata merge -sparse "${profraw_files[@]}" -o "$PROFDATA"

printf 'Writing coverage summaries...\n'
xcrun llvm-cov report "$TEST_BINARY" \
    -instr-profile="$PROFDATA" \
    -ignore-filename-regex="$IGNORE_REGEX" \
    > "$SUMMARY_TXT"

xcrun llvm-cov export "$TEST_BINARY" \
    -instr-profile="$PROFDATA" \
    -ignore-filename-regex="$IGNORE_REGEX" \
    -summary-only \
    > "$SUMMARY_JSON"

printf 'Generating HTML coverage report...\n'
xcrun llvm-cov show "$TEST_BINARY" \
    -instr-profile="$PROFDATA" \
    -ignore-filename-regex="$IGNORE_REGEX" \
    -format=html \
    -output-dir="$HTML_DIR"

printf '\nCoverage summary\n'
cat "$SUMMARY_TXT"

printf '\nArtifacts\n'
printf '  text:  %s\n' "$SUMMARY_TXT"
printf '  json:  %s\n' "$SUMMARY_JSON"
printf '  html:  %s/index.html\n' "$HTML_DIR"
printf '  data:  %s\n' "$PROFDATA"
printf '  root:  %s\n' "$SOURCE_DIR"
