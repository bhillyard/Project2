#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"

// Define the structure for a node in the linked list
typedef struct Node {
    thread thread;
    struct Node* next;
} Node;

// Initialize global head
Node *head = NULL;

// Function to print the linked list
void printLinkedList(Node* head) {
    Node* current = head;

    while (current != NULL) {
        printf("%d ", current->thread->tid);
        current = current->next;
    }

    printf("\n");
}

void rr_init(void){
}

void rr_shutdown(void){
    Node* current = head;
    Node* next;

    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

void rr_admit(thread new){
    Node* new_node = malloc(sizeof(Node));
    Node* current;
    new_node->thread = new;
    printf("thread is %d going into scheduler\n", new->tid);
    new_node->next = NULL;

    if (head == NULL) {
        printf("resetting head\n");
        head = new_node;
    } else {
        current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    printLinkedList(head);
}

// Function to delete a node at a specific index in the linked list
void rr_remove(thread thread) {
    printLinkedList(head);
    printf("\n\n\n\n\n\n\n");
    // Check if the list is empty
    if (head == NULL) {
        printf("List is empty.\n");
        return;
    }

    // If the thread is the first one in the schedule, remove the first thread
    if (head->thread == thread) {
        Node* temp = head;
        head = head->next;
        free(temp);
        return;
    }

    // Traverse the list to find the given thread
    Node *current = head;
    Node *previous = NULL;

    while (current != NULL && current->thread != thread) {
        previous = current;
        current = current->next;
    }

    // If the index is out of range
    if (current == NULL) {
        printf("No such thread in schedule.\n");
        return;
    }

    // Delete the thread at the specified thread
    previous->next = current->next;
    free(current);
}

thread rr_next(){
    if (head == NULL){
        return NULL;
    } else {
        printf("head theadis is %d\n", head->thread->tid);
        printf("len is %d\n", rr_qlen());
        printLinkedList(head);
        return head->thread;
    }
}

int rr_qlen(){
    int count = 0;
    Node* current = head;
    while (current != NULL){
        current = current->next;
        count+=1;
    }
    return count;
}

struct scheduler rr = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};
scheduler RoundRobin = &rr;
//dont need rr_init, rr_shutdown