#!/bin/sh
set -eu
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -pthread \
    *.c -o bmsv21 -pthread
echo "Built: ./bmsv21"
