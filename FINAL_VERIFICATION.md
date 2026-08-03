# Final Verification

No application features or role workflows were intentionally changed in this pass. The changes were limited to static-analysis cleanup and safer variable scoping.

## Executed checks

1. GCC warning build:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow *.c -o bms
```

Result: passed with no compiler diagnostics.

2. Clang Static Analyzer:

```bash
for f in *.c; do clang --analyze -std=c11 -Wall -Wextra "$f" -I.; done
```

Result: passed with no analyzer diagnostics.

3. AddressSanitizer, LeakSanitizer and UndefinedBehaviorSanitizer smoke test:

```bash
gcc -std=c11 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer *.c -o bms_san
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ./bms_san
```

Result: first-run administrator setup and clean exit completed without sanitizer errors or reported leaks.

## Cppcheck command

```bash
cppcheck --enable=all --std=c11 --suppress=missingIncludeSystem . 2> cppcheck_report.txt
```

## Valgrind command

Run this on the Linux server where Valgrind is installed:

```bash
gcc -std=c11 -g -O0 *.c -o bms
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./bms 2> valgrind_report.txt
```

Valgrind itself was not available in the artifact-generation container. The sanitizer and Clang analyzer checks above were executed as the available dynamic/static memory-safety verification.
