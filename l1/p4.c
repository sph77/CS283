#include <stdlib.h>
#include <stdio.h>

// Based on https://en.wikipedia.org/wiki/Bubble_sort but highly modified

typedef struct ListNode // Struct defining a linked list node (singly linked)
{
   int data;
   struct ListNode* next;
} ListNode;

void swap(ListNode* e1_prev, ListNode* e1, ListNode* e2) // Generic swap for two adjacent nodes
{
   //printf("Swapping %d and %d\n", e1->data, e2->data);
   if (e1_prev != NULL) // Adjust the node prior to e1, if it exists
   {
      e1_prev->next = e2; 
   }
   e1->next = e2->next; // Adjust e1
   e2->next = e1; // Adjust e2
}

void sort(ListNode** head) // Implementation of bubblesort for singly linked list
{
   ListNode* curr, * next, * prev; // Three rolling position trackers
   int swapped = 1;
   while (swapped) // Continue until there are no more swaps left
   {
      swapped = 0;
      curr = *head; // Start at the head
      next = curr->next;
      prev = NULL;
      while (next != NULL) // Iterate through entire list
      {
         if (curr->data > next->data) // Swap adjacent nodes that are not in order
         {
            if (curr == *head) // Update the current head if the head is swapped
            {
               *head = next;
            }
            swapped = 1;
            swap(prev, curr, next); // After swap, stay on current node
            prev = next; // The node that was next is now the previous node after swap
         }
         else // If no swap, adjust nodes normally
         {
            prev = curr; // current node is now previous
            curr = next; // next node is now current
         }
         next = curr->next; // Move the next node pointer
      }
   }
}

int main()
{
   ListNode* curr, * next, * prev;
   ListNode** list;
   int i;
   list = (ListNode**) malloc(sizeof(ListNode*));
   printf("Unsorted List\n");
   curr = (ListNode*) malloc(sizeof(ListNode));
   *list = curr;
   // Generate a random, unsorted linked list
   for (i = 0; i < 10; i++)
   {
      if (i < 9)
      {
         next = (ListNode*) malloc(sizeof(ListNode));
         curr->next = next;
      }
      else
      {
         curr->next = NULL;
      }
      curr->data = rand() % 100 + 1;
      printf("ListNode #%d = %d\n", i, curr->data);
      curr = next;
   }
   
   sort(list); // Sort in-place
   curr = *list;
   printf("Sorted List\n");
   // Print sorted list
   for (int i = 0; i < 10; i++)
   {
      printf("ListNode #%d = %d\n", i, curr->data);
      prev = curr;
      curr = curr->next;
      free(prev); // Free each node
   }
   free(list); // Free the list pointer

   return 0;
}
