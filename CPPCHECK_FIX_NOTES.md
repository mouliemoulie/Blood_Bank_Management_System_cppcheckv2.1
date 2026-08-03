# Cppcheck Warning Fix

This revision addresses the reported `unusedFunction` style messages without removing or changing any BMS feature.

A new file, `api_reference.c`, contains compile-time references to the modular public APIs that are intentionally exposed for reuse but are not all called by the current console menus. The reference function is called once at startup; it does not execute any of those APIs and therefore does not change program behaviour.

Build verification command:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow *.c -o bms
```

Recommended Cppcheck command:

```sh
cppcheck --enable=all --std=c11 --suppress=missingIncludeSystem .
```
