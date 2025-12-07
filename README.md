# linked-list-insertion-sort-cpp

A tiny, well-commented C++ program that implements stable insertion sort on a singly linked list. It’s written for learning: clear names, ASCII diagrams, and a step-by-step walkthrough.

https://github.com/user-attachments/assets/b9866cb8-3f4c-4721-86c3-14c54cd90056


## What you’ll learn
- How a singly linked list is wired: head, next, nodes
- Insertion sort as “growing a sorted prefix” (like sorting playing cards)
- Pointer hygiene: saving the next node before moving, unlinking, reinserting
- Why this version is stable and O(n²) (and when that’s OK)


## How it works

The algorithm grows a sorted region from left to right:

1. Start with the first node as the "sorted" portion (left side)
2. Take the first node from the "unsorted" portion (right side)
3. Find where it belongs in the sorted portion and insert it there
4. Repeat until no unsorted nodes remain

Like sorting a hand of cards: pick up one card at a time and slide it into the right spot among the cards you're already holding.

## Key invariants (the golden rules)
1. prev always points to the last node of the sorted prefix.
2. curr is the first node of the unsorted part (the one we’re placing).
3. We must save next = curr->next before moving curr.
4. If curr already belongs at the end of the sorted part, just advance prev. Otherwise, unlink curr and re-insert it earlier (possibly at the head).

These rules are exactly what the implementation in `src/main.cpp` does.


##  Why this is stable

FindInsertionSpot scans while `curr->data < value` (strictly `<`, not `<=`). So if two equal values appear, the earlier one stays earlier. That’s stability.


## Walkthrough on a small list

#### Start list:

```
[39] -> [45] -> [11] -> [22]
  ^        ^
 prev     curr
```

**We maintain: sorted = head..prev, unsorted = curr..end**

#### Iteration 1
- prev = 39, curr = 45, next = 11
- Find spot for 45 in [39] → spot is 39 (the last < 45)
- spot == prev → already in correct place; grow sorted region:

```
sorted:   [39] -> [45]
unsorted: [11] -> [22]
             ^      ^
            curr   next (saved)
```

**Advance: prev = 45, curr = 11.**

#### Iteration 2
- prev = 45, curr = 11, next = 22
- Find spot for 11 in [39 -> 45] → spot is nullptr (new smallest)
- spot != prev → move needed:
  1. ListRemoveAfter(list, prev) unlinks 11 from after 45
  2. ListPrepend(list, curr) inserts 11 at the head

```
[11] -> [39] -> [45] -> [22]
         ^         ^
       head       prev (unchanged)
```

**Advance: curr = 22 (the next we saved earlier).**

#### Iteration 3
- prev = 45, curr = 22, next = nullptr
- Find spot for 22 in [11 -> 39 -> 45] → spot is the node 11 (since 11 < 22 and 39 !< 22)
- spot != prev → move needed:
  1. Unlink 22 from after 45
  2. Insert after 11

```
[11] -> [22] -> [39] -> [45]
```

**Advance curr = next = nullptr → done.**

**Result is sorted and stable.**


## Code structure

- **Node, List**: minimal data structures (head pointer + next links)
- **ListPrepend, ListInsertAfter, ListRemoveAfter**: core list operations
- **FindInsertionSpot**: scans the sorted portion to find where a value belongs
- **ListInsertionSort**: the main algorithm that ties everything together


## Complexity
- Time: O(n²) comparisons/moves in the worst case (like array insertion sort)
- Space: O(1) extra space (we move nodes in place)
- Good when you want stable behavior and cheap insertion without shifting elements


## Build & run

### Makefile (recommended)
```bash
make run              # normal mode
make run TRACE=1      # visual trace mode
make clean            # remove binary
```

### CMake
```bash
# Normal mode
cmake -S . -B build && cmake --build build
./build/linked_list_insertion_sort

# Visual trace mode
cmake -S . -B build -DTRACE=ON && cmake --build build
./build/linked_list_insertion_sort
```

### Direct compile
```bash
# clang++ (default on macOS)
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -Iinclude src/main.cpp -o ll_isort
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -Iinclude -DTRACE src/main.cpp -o ll_isort  # trace

# g++
g++ -std=c++20 -O2 -Wall -Wextra -pedantic -Iinclude src/main.cpp -o ll_isort
g++ -std=c++20 -O2 -Wall -Wextra -pedantic -Iinclude -DTRACE src/main.cpp -o ll_isort      # trace

./ll_isort
```

### Helper scripts
```bash
# macOS/Linux
./scripts/run.sh            # normal mode
./scripts/run.sh trace      # visual trace mode

# Windows (Command Prompt)
scripts\run.bat
scripts\run.bat trace

# Windows (PowerShell)
.\scripts\run.ps1
.\scripts\run.ps1 -Trace
```

### Expected output

**Normal mode:**
```
=== Linked List Insertion Sort ===
Input:  39 -> 45 -> 11 -> 22
Output: 11 -> 22 -> 39 -> 45

Algorithm: O(n^2) time, O(1) space, stable
```

### Visual trace (ANSI borders + colors)

With TRACE enabled, each step prints an ANSI-bordered box with the list and role markers. Nodes with multiple roles show them separated by slashes (e.g., `H/P/S`), each letter in its own color. Colors auto-disable if not a TTY.

#### Example (your terminal will show colors):

```
┌────────────────────────────────────┐
│ BEFORE place                       │
├────────────────────────────────────┤
│   [39] -> [45] -> [11] -> [22]     │
│   H/P/S     C       N              │
├────────────────────────────────────┤
│ H=head P=prev C=curr N=next S=spot │
└────────────────────────────────────┘

┌────────────────────────────────────┐
│ AFTER unlink                       │
├────────────────────────────────────┤
│   [39] -> [45] -> [22]             │
│     H       P       N              │
│ C (isolated): [11]                 │
├────────────────────────────────────┤
│ H=head P=prev C=curr N=next S=spot │
└────────────────────────────────────┘

┌────────────────────────────────────┐
│ AFTER insert (at head)             │
├────────────────────────────────────┤
│   [11] -> [39] -> [45] -> [22]     │
│    H/C              P       N      │
├────────────────────────────────────┤
│ H=head P=prev C=curr N=next S=spot │
└────────────────────────────────────┘

Sorted: 11 -> 22 -> 39 -> 45
```

#### Legend
- H = head node
- P = prev (end of sorted prefix)
- C = curr (node being placed)
- N = next (saved pointer for the next loop)
- S = spot (node after which `curr` will be inserted; S at head means insert at head)

#### Notes
- Colors are 256-color safe and disabled when stdout is not a TTY (e.g., piped to file).
- The bordered state is generated by `include/trace_ui.hpp` and integrated into `src/main.cpp` under `#ifdef TRACE`.


### Teaching notes
- The code prioritizes clarity over performance tricks
- Each list operation (prepend, insert, remove) is separate and can be tested on its own
- The trace shows what each pointer is doing at every step
- Stability: we use `<` (not `<=`) when scanning, so equal values stay in their original order


### Common pitfalls (and how this code avoids them)
- Losing the rest of the list: Always set `Node* next = curr->next;` before you move `curr`.
- Breaking links: `ListInsertAfter` sets `newNode->next = prev->next` before `prev->next = newNode`.
- Head updates: When inserting at the front, use `ListPrepend` (it updates `head`).
- Scanning into unsorted area: `FindInsertionSpot` stops at `boundary` to keep the search in the sorted prefix.
- Advancing `prev` incorrectly: Only advance `prev` if no move occurred (i.e., `spot == prev`).


### Exercises
1. Support any data type (not just `int`) by making the code use templates
2. Add more list operations like `PushFront`, `PopFront`, `PopAfter`
3. Write tests with random data and compare results to `std::stable_sort`
4. Test stability: use duplicate values and check that equal items keep their original order


### File tree

```
.
├── CMakeLists.txt        # CMake build file (TRACE toggle supported)
├── Makefile              # make run | make run TRACE=1 | make clean
├── README.md             # This file
├── .gitignore            # Ignores build artifacts and IDE files
├── src/
│   └── main.cpp          # Full implementation + demo + trace hooks
├── include/
│   └── trace_ui.hpp      # ANSI-colored, bordered trace UI for the linked list
├── docs/
│   └── pseudo.cpp        # Pseudocode-style reference (not compiled)
└── scripts/
    ├── run.sh            # macOS/Linux helper (use: ./scripts/run.sh [trace])
    ├── run.bat           # Windows CMD helper (use: scripts\run.bat [trace])
    └── run.ps1           # Windows PowerShell helper (use: .\scripts\run.ps1 [-Trace])
```


### Function reference (quick)

- `struct Node { int data; Node* next; }`
  - A single list node. `data` holds the value, `next` points to the next node or `nullptr`.
- `struct List { Node* head; }`
  - Holds a pointer to the first node. `head == nullptr` means empty list.
- `void ListPrepend(List* list, Node* newNode)`
  - Inserts `newNode` at the front. Updates `list->head`.
- `void ListInsertAfter(List* /*list*/, Node* prev, Node* newNode)`
  - Inserts `newNode` immediately after `prev`. Requires `prev != nullptr`.
- `Node* ListRemoveAfter(List* list, Node* prev)`
  - Removes and returns the node after `prev`. If `prev == nullptr`, removes the head. Safely isolates the removed node’s `next`.
- `Node* FindInsertionSpot(const List* list, int value, Node* boundary)`
  - Scans from `list->head` up to (but not including) `boundary` and returns the node after which `value` should be inserted. Returns `nullptr` if it should go at the head.
- `void ListInsertionSort(List* list)`
   - Stable, in-place insertion sort: grows a sorted prefix and inserts each `curr` into the correct spot.
- `void PushBack(List* list, int data)`
  - Test helper: append a new node to the end.
- `void PrintList(const List* list)`
  - Prints values like `1 -> 2 -> 3`.

### Trace UI reference

- Header: `include/trace_ui.hpp` (enabled only when `TRACE` is defined)
  - `traceui::print_state(title, head, roles)` prints a bordered, colored snapshot.
  - Roles struct: `traceui::PtrRoles<Node>{H,P,C,N,S}` marks head, prev, curr, next, spot.
  - Palette constants (256-color): `C_HEAD, C_PREV, C_CURR, C_NEXT, C_SPOT, C_TEXT, C_BORDER`.
  - Colors auto-disable if stdout is not a TTY.

### Troubleshooting

- Compiler not found on Windows
  - Use `run.bat` or `run.ps1` with g++/clang++ installed (MSYS2/MinGW or LLVM). Or use CMake + Visual Studio generator.
- Box characters look wrong
  - Ensure your terminal uses UTF-8 and a monospace font. Colors may be disabled if piping to a file.
- No colors shown
  - Colors only enable when writing to an interactive terminal (TTY). They are intentionally off when redirected.
