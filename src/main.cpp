#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>

#ifdef TRACE
#include "../include/trace_ui.hpp"
#endif

/*
 * =============================================================================
 * Data Structures
 * =============================================================================
 */

/**
 * A Node is a single box in our list.
 * It holds a piece of data and an arrow (pointer) to the next box.
 *
 * VISUAL:
 *   ┌───────┬────────┐
 *   │ data  │ next*  │
 *   └───────┴────────┘
 */
struct Node {
    int data;
    Node* next;
    explicit Node(int d) : data(d), next(nullptr) {}
};

/**
 * The List is just a signpost that points to the very first node (the head).
 * If the list is empty, the head signpost points to nothing (nullptr).
 *
 * VISUAL:
 *   ┌───────┐
 *   │ head* │
 *   └───────┘
 */
struct List {
    Node* head;
    List() : head(nullptr) {}
};

/**
 * Optional teaching aid: print the list after important steps.
 * Only prints when NOT using TRACE (TRACE has its own visual output).
 */
static void DebugPrint([[maybe_unused]] const char* msg, [[maybe_unused]] const List* list) {
    /* When TRACE is enabled, we use the visual boxes instead */
}

/*
 * =============================================================================
 * Basic List Operations
 * =============================================================================
 */

/**
 * ListPrepend - Puts a new node at the very front of the list, making it the new head.
 */
void ListPrepend(List* list, Node* newNode) {
    /*
     * BEFORE:
     *   head* -> [ A ] -> [ B ]
     *
     *   [ newNode ]  (floating by itself)
     */

    /* 1. Make the new node's arrow point to the current first node (A). */
    newNode->next = list->head;
    /*
     * AFTER STEP 1:
     *   head* -> [ A ] -> [ B ]
     *            ^
     *            |
     *   [ newNode ]
     */

    /* 2. Change the list's head signpost to point to our new node. */
    list->head = newNode;
    /*
     * FINAL RESULT:
     *   head* -> [ newNode ] -> [ A ] -> [ B ]
     */
}

/**
 * ListInsertAfter - Puts newNode into the list immediately after the prev node.
 */
void ListInsertAfter(List* /*list*/, Node* prev, Node* newNode) {
    assert(prev != nullptr && "Cannot insert after a null node");
    /*
     * BEFORE:
     *   ... -> [ prev ] -> [ C ] -> ...
     *
     *   [ newNode ]  (floating by itself)
     */

    /* 1. Make the new node point to whatever prev was pointing at (C).
     *    We do this first so we don't lose the rest of the list. */
    newNode->next = prev->next;
    /*
     * AFTER STEP 1:
     *   ... -> [ prev ] -> [ C ] -> ...
     *                     ^
     *                     |
     *          [ newNode ]
     */

    /* 2. Now, make prev point to the new node. This completes the link. */
    prev->next = newNode;
    /*
     * FINAL RESULT:
     *   ... -> [ prev ] -> [ newNode ] -> [ C ] -> ...
     */
}

/**
 * ListRemoveAfter - Removes the node that comes right after prev.
 */
Node* ListRemoveAfter(List* list, Node* prev) {
    /*
     * --- SPECIAL CASE: Remove the head node (prev is nullptr) ---
     */
    if (prev == nullptr) {
        Node* nodeToRemove = list->head;  /* The head is the one to remove. */
        /*
         * BEFORE:
         *   head* -> [ A ] -> [ B ] -> ...
         *            (nodeToRemove)
         */
        if (nodeToRemove) {
            /* Make the list's head signpost point to the second node (B). */
            list->head = nodeToRemove->next;
            /* Isolate the old head node completely. */
            nodeToRemove->next = nullptr;
        }
        /*
         * FINAL RESULT:
         *   head* -> [ B ] -> ...
         *
         *   [ A ]  (removed)
         */
        return nodeToRemove;
    }

    /*
     * --- NORMAL CASE: Remove the node after prev ---
     */
    Node* nodeToRemove = prev->next;
    /*
     * BEFORE:
     *   ... -> [ prev ] -> [ B ] -> [ C ] -> ...
     *                      (nodeToRemove)
     */
    if (nodeToRemove) {
        /* Make prev's arrow skip over B and point directly to C. */
        prev->next = nodeToRemove->next;
        /* Isolate the removed node completely. */
        nodeToRemove->next = nullptr;
    }
    /*
     * FINAL RESULT:
     *   ... -> [ prev ] -> [ C ] -> ...
     *
     *   [ B ]  (removed)
     */
    return nodeToRemove;
}

/**
 * FindInsertionSpot - Finds the node that should come right BEFORE a new value
 * in the sorted part of the list. Returns nullptr if the value should be the new head.
 */
Node* FindInsertionSpot(const List* list, int value, Node* boundary) {
    Node* prev = nullptr;
    Node* curr = list->head;

    /*
     * Scan the list. curr moves forward, prev follows one step behind.
     * Stop when curr hits the boundary or finds a value bigger than our new one.
     *
     * EXAMPLE: Find spot for 22 in [ 11 -> 39 -> 45 ]. Boundary is nullptr (end of list).
     *
     * 1. curr=11. 11 < 22. prev becomes 11, curr becomes 39.
     * 2. curr=39. 39 is NOT < 22. Loop stops.
     *
     * Function returns prev, which is the node containing 11.
     * This tells us: "Insert 22 AFTER the node with 11".
     */
    while (curr != boundary && curr->data < value) {
        prev = curr;
        curr = curr->next;
    }
    return prev;
}

/**
 * ListInsertionSort - Sorts the list using the insertion sort method.
 *
 * Time: O(n^2), Space: O(1), Stable: Yes
 */
void ListInsertionSort(List* list) {
    /* A list with 0 or 1 nodes is already sorted. */
    if (!list || !list->head || !list->head->next) {
        return;
    }

    /* prev is the last node in the sorted part. It starts at the head. */
    Node* prev = list->head;
    /* curr is the first node in the unsorted part. */
    Node* curr = prev->next;
    /*
     * INITIAL STATE: [ 39 ] -> [ 45 ] -> [ 11 ] -> [ 22 ]
     *                  ^         ^
     *                 prev      curr
     */

    while (curr != nullptr) {
        /* Teaching hook: show the current state before placing curr. */
        DebugPrint("Before placing curr", list);
        /* Save the next node in the list before we start moving curr. */
        Node* next = curr->next;

        /* Find the spot where curr belongs in the sorted part. */
        Node* spot = FindInsertionSpot(list, curr->data, /*boundary=*/curr);

#ifdef TRACE
        traceui::print_state("BEFORE place",
                             list->head,
                             traceui::PtrRoles<Node>{list->head, prev, curr, next, spot});
#endif

        /*
         * --- CASE 1: curr is already in the right place. ---
         * This happens if its value is the largest so far.
         */
        if (spot == prev) {
            /* The sorted part just grows by one. We advance both pointers. */
            prev = curr;
        }
        /*
         * --- CASE 2: curr needs to be moved. ---
         */
        else {
            /*
             * EXAMPLE: curr is 11. Sorted part is [ 39 -> 45 ]. prev is 45.
             * FindInsertionSpot returns nullptr for 11.
             *
             * STEP A: Unlink curr from the list.
             */
            ListRemoveAfter(list, prev);
            DebugPrint("Unlinked curr", list);
#ifdef TRACE
            traceui::print_state("AFTER unlink",
                                 list->head,
                                 traceui::PtrRoles<Node>{list->head, prev, curr, next, spot},
                                 curr);  /* Pass isolated node to show */
#endif
            /*
             * VISUAL after unlinking:
             *   [ 39 ] -> [ 45 ] -> [ 22 ]   (prev (45) now points to next)
             *
             *   [ 11 ] (this is curr, now isolated)
             */

            /* STEP B: Re-insert curr at the correct spot. */
            if (spot == nullptr) {
                /* If spot is null, curr is the new smallest item. Put it at the front. */
                ListPrepend(list, curr);
                DebugPrint("Inserted curr", list);
#ifdef TRACE
                traceui::print_state("AFTER insert (at head)",
                                     list->head,
                                     traceui::PtrRoles<Node>{list->head, prev, curr, next, spot});
#endif
                /*
                 * VISUAL after re-inserting:
                 *   [ 11 ] -> [ 39 ] -> [ 45 ] -> [ 22 ]
                 */
            } else {
                /* Otherwise, insert curr after the spot we found. */
                ListInsertAfter(list, spot, curr);
                DebugPrint("Inserted curr", list);
#ifdef TRACE
                traceui::print_state("AFTER insert at spot",
                                     list->head,
                                     traceui::PtrRoles<Node>{list->head, prev, curr, next, spot});
#endif
            }
            /*
             * IMPORTANT: prev does NOT change in this case because we removed
             * the node that was after it. It still correctly points to the end
             * of the chain of sorted nodes.
             */
        }

        /* Move to the next unsorted node and repeat the process. */
        curr = next;
    }
}

/*
 * =============================================================================
 * Test Functions
 * =============================================================================
 */

/** Adds a new node to the end of the list. */
void PushBack(List* list, int data) {
    Node* n = new Node(data);
    if (!list->head) {
        list->head = n;
        return;
    }
    Node* curr = list->head;
    while (curr->next) curr = curr->next;
    curr->next = n;
}

/** Prints the list to the console. */
void PrintList(const List* list) {
    const Node* curr = list->head;
    while (curr) {
        std::cout << curr->data;
        if (curr->next) std::cout << " -> ";
        curr = curr->next;
    }
    std::cout << '\n';
}

/** Main function to run the test. */
int main() {
    List mylist;
    PushBack(&mylist, 39);
    PushBack(&mylist, 45);
    PushBack(&mylist, 11);
    PushBack(&mylist, 22);

#ifndef TRACE
    std::cout << "=== Linked List Insertion Sort ===\n";
    std::cout << "Input:  ";
    PrintList(&mylist);
#endif

    ListInsertionSort(&mylist);

#ifndef TRACE
    std::cout << "Output: ";
    PrintList(&mylist);  /* Should print: 11 -> 22 -> 39 -> 45 */
    std::cout << "\nAlgorithm: O(n^2) time, O(1) space, stable\n";
#else
    std::cout << "\nSorted: ";
    PrintList(&mylist);
#endif

    /* Clean up memory. */
    Node* curr = mylist.head;
    while (curr != nullptr) {
        Node* to_delete = curr;
        curr = curr->next;
        delete to_delete;
    }

    return 0;
}
