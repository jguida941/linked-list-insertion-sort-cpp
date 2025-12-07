/**
 * @brief Sorts a singly linked list using a stable insertion sort.
 * @details The algorithm maintains a sorted prefix and iteratively inserts
 * elements from the unsorted part into their correct position.
 * Time complexity: O(n^2). Space complexity: O(1).
 * @param list A pointer to the list to be sorted.
 */
void ListInsertionSort(List* list) {
    // If the list has 0 or 1 nodes, it's already sorted.
    if (!list || !list->head || !list->head->next) {
        return;
    }

    // `prev_node` is the last node in the sorted part of the list.
    Node* prev_node = list->head;
    // `next_node` is the first node we need to place into the sorted part.
    Node* next_node = prev_node->next;

    // Loop until we have sorted every node.
    while (next_node != NULL) {
        // IMPORTANT: Save the next node to process before we move the current one.
        Node* next_temp = next_node->next;

        // Find where `next_node` belongs in the already sorted section.
        // The search should scan from the head up to (but not including) `next_node`.
        Node* insert_after = FindInsertionSpot(list, next_node->data, /*boundary=*/next_node);

        // CASE 1: The node is already in the correct spot.
        if (insert_after == prev_node) {
            // The node stays put. The sorted section just gets one node longer.
            prev_node = next_node;
        }
        // CASE 2: The node needs to be moved.
        else {
            // Unlink the node from its current position.
            ListRemoveAfter(list, prev_node);

            // If the spot is NULL, the node is the new smallest item.
            if (insert_after == NULL) {
                ListPrepend(list, next_node);
            }
            // Otherwise, insert it right after the spot we found.
            else {
                ListInsertAfter(list, insert_after, next_node);
            }
        }

        // Move on to the next unsorted node that we saved earlier.
        next_node = next_temp;
    }
}