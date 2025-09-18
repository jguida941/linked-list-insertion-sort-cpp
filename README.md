linked-list-insertion-sort-cpp

A tiny, well-commented C++ program that implements stable insertion sort on a singly linked list. It’s written for learning: clear names, ASCII diagrams, and a step-by-step walkthrough.

What you’ll learn
- How a singly linked list is wired: head, next, nodes
- Insertion sort as “growing a sorted prefix” (like sorting playing cards)
- Pointer hygiene: saving the next node before moving, unlinking, reinserting
- Why this version is stable and O(n²) (and when that’s OK)

⸻

Mental model (plain English)
- Keep the left side of the list sorted.
- Take the first node on the right (“unsorted”), find where it belongs inside the left part, and insert it there.
- Repeat until there’s no right part left.

Think of holding a fan of playing cards in your left hand and inserting the next card into its place.

⸻

Key invariants (the golden rules)
1. prev always points to the last node of the sorted prefix.
2. curr is the first node of the unsorted part (the one we’re placing).
3. We must save next = curr->next before moving curr.
4. If curr already belongs at the end of the sorted part, just advance prev. Otherwise, unlink curr and re-insert it earlier (possibly at the head).

These rules are exactly what the implementation in main.cpp does.

⸻

Why this is stable

FindInsertionSpot scans while `curr->data < value` (strictly `<`, not `<=`). So if two equal values appear, the earlier one stays earlier. That’s stability.

⸻

Walkthrough on a small list

Start list:

```
[39] -> [45] -> [11] -> [22]
  ^        ^
 prev     curr
```

We maintain: sorted = head..prev, unsorted = curr..end.

Iteration 1
- prev = 39, curr = 45, next = 11
- Find spot for 45 in [39] → spot is 39 (the last < 45)
- spot == prev → already in correct place; grow sorted region:

```
sorted:   [39] -> [45]
unsorted: [11] -> [22]
             ^      ^
            curr   next (saved)
```

Advance: prev = 45, curr = 11.

Iteration 2
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

Advance: curr = 22 (the next we saved earlier).

Iteration 3
- prev = 45, curr = 22, next = nullptr
- Find spot for 22 in [11 -> 39 -> 45] → spot is the node 11 (since 11 < 22 and 39 !< 22)
- spot != prev → move needed:
  1. Unlink 22 from after 45
  2. Insert after 11

```
[11] -> [22] -> [39] -> [45]
```

Advance curr = next = nullptr → done.

Result is sorted and stable.

⸻

File tour (what does what)
- Node, List: minimal data structures (head pointer + next links)
- ListPrepend, ListInsertAfter, ListRemoveAfter: tiny, testable building blocks
- FindInsertionSpot(list, value, boundary): scans only within the sorted prefix (from head up to boundary), returning the node after which to insert (or nullptr for “insert at head”)
- ListInsertionSort: the algorithm glue that uses the helpers safely

⸻

Complexity
- Time: O(n²) comparisons/moves in the worst case (like array insertion sort)
- Space: O(1) extra space (we move nodes in place)
- Good when you want stable behavior and cheap insertion without shifting elements

⸻

Build & run

Quick start (Makefile)
```
# Default build and run
make run

# With teaching trace enabled
make run TRACE=1
```

macOS/Linux direct compile (no Make/CMake)
```
# Normal
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic main.cpp -o ll_isort && ./ll_isort

# With visual TRACE
clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -DTRACE main.cpp -o ll_isort && ./ll_isort

# If you prefer g++
g++ -std=c++20 -O2 -Wall -Wextra -pedantic main.cpp -o ll_isort && ./ll_isort
g++ -std=c++20 -O2 -Wall -Wextra -pedantic -DTRACE main.cpp -o ll_isort && ./ll_isort
```

Windows quick start
```
:: Command Prompt (.bat)
run.bat
run.bat trace    :: enable teaching trace

# PowerShell
./run.ps1
./run.ps1 -Trace
```

Option A: CMake
```
cmake -S . -B build
cmake --build build
./build/linked_list_insertion_sort
```

Option B: One-liner
```
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o ll_isort
./ll_isort
```

Expected output:

```
Before sort: 39 -> 45 -> 11 -> 22
After  sort: 11 -> 22 -> 39 -> 45
```

⸻

(Optional) Trace mode for teaching

Trace hooks are already built into main.cpp. Enabling TRACE prints both a textual trace and a visual two‑line ASCII diagram that marks `head`, `prev`, `curr`, `next`, and `spot`.

Enable TRACE:

```
# CMake
cmake -S . -B build -DTRACE=ON && cmake --build build && ./build/linked_list_insertion_sort

# Makefile
make run TRACE=1

# Direct compile
g++ -std=c++20 -O2 -Wall -Wextra -pedantic -DTRACE main.cpp -o ll_isort && ./ll_isort
```

⸻

Visual trace (ANSI borders + colors)

With TRACE enabled, each step prints an ANSI-bordered box with the list and role markers (H,P,C,N,S). Colors auto-disable if not a TTY.

Example (your terminal will show colors):

```
╔══════════════════════════════════════════════════════════════╗
║ State: BEFORE place (H=head, P=prev, C=curr, N=next, S=spot) ║
║ [39] -> [45] -> [11] -> [22]                                 ║
║     H       P       C       N                                 ║
╚══════════════════════════════════════════════════════════════╝

╔═════════════════════════════════════╗
║ State: AFTER unlink (curr isolated) ║
║ [39] -> [45] -> [22]                ║
║     H       P     N                 ║
╚═════════════════════════════════════╝

C (isolated): [11]

╔════════════════════════════════════╗
║ State: AFTER insert after S (spot) ║
║ [11] -> [22] -> [39] -> [45]       ║
║     S       C             P        ║
╚════════════════════════════════════╝
```

Legend
- H = head node
- P = prev (end of sorted prefix)
- C = curr (node being placed)
- N = next (saved pointer for the next loop)
- S = spot (node after which `curr` will be inserted; S at head means insert at head)

Notes
- Colors are 256-color safe and disabled when stdout is not a TTY (e.g., piped to file).
- The bordered state is generated by `trace_ui.hpp` and integrated into `main.cpp` under `#ifdef TRACE`.

⸻

Educational purpose and teaching notes
- The code favors readability and pointer hygiene over micro-optimizations.
- Each operation is isolated (prepend/insert/remove) so students can test them independently.
- The trace shows the invariants in action: prev marks the end of the sorted prefix; curr is the node being placed; next is saved before rewiring; spot marks where curr re-enters.
- Stability comes from scanning with `<` (not `<=`), so equal keys keep their original relative order.

⸻

Common pitfalls (and how this code avoids them)
- Losing the rest of the list: Always set `Node* next = curr->next;` before you move `curr`.
- Breaking links: `ListInsertAfter` sets `newNode->next = prev->next` before `prev->next = newNode`.
- Head updates: When inserting at the front, use `ListPrepend` (it updates `head`).
- Scanning into unsorted area: `FindInsertionSpot` stops at `boundary` to keep the search in the sorted prefix.
- Advancing `prev` incorrectly: Only advance `prev` if no move occurred (i.e., `spot == prev`).

⸻

Exercises
1. Make it generic: turn `int` into a templated `T` with a comparator.
2. Add `PushFront`, `PopFront`, `PopAfter` to practice pointer edits.
3. Create randomized tests that compare to `std::stable_sort` on a vector as an oracle.
4. Show stability: add duplicates and verify the original order of equals is preserved.

⸻

File tree

```
.
├── CMakeLists.txt  # Minimal CMake build file (TRACE toggle supported)
├── Makefile        # make run | make run TRACE=1 | make clean
├── main.cpp        # Full implementation + demo + trace hooks
├── pseudo.cpp      # Pseudocode-style reference (not compiled)
├── trace_ui.hpp    # ANSI-colored, bordered trace UI for the linked list
├── run.bat         # Windows CMD helper (use: run.bat [trace])
├── run.ps1         # Windows PowerShell helper (use: ./run.ps1 [-Trace])
└── .gitignore      # Ignores build artifacts and IDE files
```

⸻

Function reference (quick)

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
  - Stable, in‑place insertion sort: grows a sorted prefix and inserts each `curr` into the correct spot.
- `void PushBack(List* list, int data)`
  - Test helper: append a new node to the end.
- `void PrintList(const List* list)`
  - Prints values like `1 -> 2 -> 3`.

Trace UI reference

- Header: `trace_ui.hpp` (enabled only when `TRACE` is defined)
  - `traceui::print_state(title, head, roles)` prints a bordered, colored snapshot.
  - Roles struct: `traceui::PtrRoles<Node>{H,P,C,N,S}` marks head, prev, curr, next, spot.
  - Palette constants (256‑color): `C_HEAD, C_PREV, C_CURR, C_NEXT, C_SPOT, C_TEXT, C_BORDER`.
  - Colors auto‑disable if stdout is not a TTY.

Troubleshooting

- Compiler not found on Windows
  - Use `run.bat` or `run.ps1` with g++/clang++ installed (MSYS2/MinGW or LLVM). Or use CMake + Visual Studio generator.
- Box characters look wrong
  - Ensure your terminal uses UTF‑8 and a monospace font. Colors may be disabled if piping to a file.
- No colors shown
  - Colors only enable when writing to an interactive terminal (TTY). They are intentionally off when redirected.
