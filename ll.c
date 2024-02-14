#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"

// Node structure
typedef struct Node {
    thread thread;
    struct Node* next;
} Node;


// Linked list structure
typedef struct LinkedList {
    Node* head;
    Node* tail;
} LinkedList;

// create a new node
Node* createNode(thread thread) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->thread = thread;
    newNode->next = NULL;
    return newNode;
}

// create a linked list
LinkedList* createLinkedList() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// always queue elements on back of list
void enqueue(LinkedList* list, int data) {
    Node* newNode = createNode(data);
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

// get head of linked list
Node* front(LinkedList* list) {
    return list->head;
}

// get length
int len(LinkedList* list) {
    int count = 0;
    Node* current = list->head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}


// print linked list
void printLinkedList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        printf("%d ", current->thread->tid);
        current = current->next;
    }
    printf("\n");
}
