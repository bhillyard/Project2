#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"

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
void enqueue(LinkedList* list, thread thread) {
    Node* newNode = createNode(thread);
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

 // get head of linked list and detach it
thread dequeue(LinkedList* list) {
    Node* head = list->head;
    if (head != NULL) {
        list->head = head->next;
        if (list->tail == head) {
            list->tail = NULL;
        }
        head->next = NULL;
        thread dequeuedThread = head->thread;
        free(head);
        return dequeuedThread;
    }
    return NULL;
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
void printList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        printf("%d ", current->thread->tid);
        current = current->next;
    }
    printf("\n");
}
