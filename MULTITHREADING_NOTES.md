# Multithreading implementation

- Main thread: user interface and all normal BMS workflows.
- Background POSIX thread: checks expired inventory and low-stock records every 30 seconds.
- Inventory mutex: protects all inventory reads, updates, traversal and persistence.
- Graceful shutdown: condition variable wakes the monitor immediately and `pthread_join` waits for completion.
- Build: `make` or `gcc -std=c11 -Wall -Wextra -Wpedantic *.c -o bmsv21 -pthread`.
- No business feature, role, menu, file format or ID format was intentionally changed.
