# Blood Bank Management System — Complete Revised Version

## OnlineGDB

1. Select **C** as the language, not C++.
2. Upload every `.c` and `.h` file into the same project.
3. Ensure `main.c` is included.
4. Press **Run**.

Local build:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow *.c -o bms
./bms
```

## Implemented revisions

- Internal creation IDs are automatically generated using highest existing ID + 1.
- Request ID is generated automatically.
- Hospital ID is generated automatically.
- Hospital Staff can register and link one hospital to their account.
- Blood-request creation obtains Hospital ID from the logged-in Hospital Staff account.
- Manual Request ID and Hospital ID entry were removed from request creation.
- Donor, inventory, donation, and emergency-alert creation IDs are automatic.
- A1+ blood group support is included.
- Hospital phone and email are validated.
- Navigation remains: `-1` previous field, `0` current menu, `-99` main menu.

IDs requested for lookup, approval, deletion, routes, or references remain user selections because they refer to already-existing records rather than create new records.
