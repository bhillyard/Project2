#include <stdio.h>
#include <stdlib.h>

// Define the structure for a node in the linked list
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Function to add a new node at the beginning of the linked list
void addToLinkedList(Node** head, int data) {
    // Create a new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = *head;

    // Update the head pointer to point to the new node
    *head = newNode;
}

// Function to delete a node at a specific index in the linked list
void deleteAtIndex(Node** head, int index) {
    // Check if the list is empty
    if (*head == NULL) {
        printf("List is empty.\n");
        return;
    }

    // Check if the index is valid
    if (index < 0) {
        printf("Invalid index.\n");
        return;
    }

    // If the index is 0, delete the first node
    if (index == 0) {
        Node* temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }

    // Traverse the list to find the node at the specified index
    Node* current = *head;
    Node* previous = NULL;
    int count = 0;

    while (current != NULL && count < index) {
        previous = current;
        current = current->next;
        count++;
    }

    // If the index is out of range
    if (current == NULL) {
        printf("Index out of range.\n");
        return;
    }

    // Delete the node at the specified index
    previous->next = current->next;
    free(current);
}

// Function to print the linked list
void printLinkedList(Node* head) {
    Node* current = head;

    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }

    printf("\n");
}

int main() {
    Node* head = NULL;

    // Add nodes to the linked list
    addToLinkedList(&head, 3);
    addToLinkedList(&head, 7);
    addToLinkedList(&head, 9);

    // Print the linked list
    printf("Linked List: ");
    printLinkedList(head);

    // Delete a node at index 1
    deleteAtIndex(&head, 1);

    // Print the updated linked list
    printf("Updated Linked List: ");
    printLinkedList(head);

    return 0;
}
