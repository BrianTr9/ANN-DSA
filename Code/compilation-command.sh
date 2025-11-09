#!/usr/bin/env bash

# Always resolve paths relative to this script's directory
SCRIPT_DIR="$(cd -- "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd -P)"

# Compile
INCLUDE1="${SCRIPT_DIR}/include"
INCLUDE2="${SCRIPT_DIR}/include/tensor"
INCLUDE3="${SCRIPT_DIR}/include/sformat"
INCLUDE4="${SCRIPT_DIR}/include/ann"
INCLUDE5="${SCRIPT_DIR}/demo"
SRC1="${SCRIPT_DIR}/src/ann"
SRC2="${SCRIPT_DIR}/src/tensor"
MAIN="${SCRIPT_DIR}/src/program.cpp"

echo "################################################"
echo "# Compilation of the assignment: STARTED #######"
echo "################################################"

# Collect sources safely even if paths contain spaces
declare -a SRCS
while IFS= read -r -d '' file; do SRCS+=("$file"); done < <(find "$SRC1" -type f -name '*.cpp' -print0)
while IFS= read -r -d '' file; do SRCS+=("$file"); done < <(find "$SRC2" -type f -name '*.cpp' -print0)

g++ -std=c++17 -I "$INCLUDE1" -I "$INCLUDE2" -I "$INCLUDE3" -I "$INCLUDE4" -I "$INCLUDE5" \
	"${SRCS[@]}" "$MAIN" -o "${SCRIPT_DIR}/program"

echo "################################################"
echo "# Compilation of the assignment: END     #######"
echo "# Binary file output: ${SCRIPT_DIR}/program ##########"
echo "################################################"