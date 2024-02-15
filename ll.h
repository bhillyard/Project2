#ifndef LL_H
#define LL_H

#include "lwp.h" // Include necessary headers for types used in linked list operations

// Linked list structure
typedef struct LinkedList {
    Node* head;
    Node* tail;
} LinkedList;

// Function prototypes
Node* createNode(thread thread);
LinkedList* createLinkedList();
void enqueue(LinkedList* list, thread thread);
thread dequeue(LinkedList* list);
int len(LinkedList* list);
void printList(LinkedList* list);

#endif