# Cppcheck second-pass fixes

The following static-analysis findings were corrected without changing BMS features:

- zero-length `calloc()` paths in module save/load functions
- possible null dereference report in linked-list sorting
- authentication variable-scope and unread-variable reports
- direct initialization of status variables before use
- local suppressions for intentional loop/navigation state variables where reducing scope would change the control-flow structure

Run from the project directory (there is no `include/` subdirectory):

```sh
cppcheck --enable=all --std=c11 --suppress=missingIncludeSystem .
```
