#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{   
    printf("*Firsthalf : %p\n",*firstHalf);
    printf("head this round: %p\n",head);

    asm volatile(
        /*
        Block A (splitList), which splits the linked list into two halves
        */

        "beq %[head],x0,Null    \n\t" //if (head is empty): jump to Null
        "ld x6,8(%[head])       \n\t" //fast x6 = *(head->next) 
        "beq x6,x0,one          \n\t" // head->next is null: jump to one
        "add x7,x0,%[head]      \n\t" //slow x7 = head
        
        //while loop
        "loop:                  \n\t"
        "beq x6, x0,out         \n\t" //if fast is null: out
        "ld x28,8(x6)           \n\t" //fast x28 = fast->next             
        "beq x28, x0,out        \n\t" //replace beq x29,x0,out

        "ld x30,8(x7)           \n\t" //*(slow->next)
        "addi x7,x30,0          \n\t" //slow = *(slow->next)
        "ld x28,8(x6)           \n\t" 
        "ld x28,8(x28)          \n\t"
        "addi x6,x28,0          \n\t" //fast = fast->next->next
        "jal x0,loop            \n\t"


        "out:                   \n\t"
        "sd %[head],0(%[firstHalf])      \n\t" //*firstHalf = head
        "ld x5,8(x7)            \n\t" //*(slow.next) to x5
        "sd x5,0(%[secondHalf])           \n\t"
        "sd x0,8(x7)            \n\t" //x0 write to address of slow.next本身
        "jal x0,Exit            \n\t"


        "one:                   \n\t"
        "sd %[head],0(%[firstHalf]) \n\t"
        "sd x0,0(%[secondHalf])     \n\t"
        "jal x0,Exit                \n\t"


        "Null:                  \n\t"
        "sd x0,0(%[firstHalf])           \n\t"
        "sd x0,0(%[secondHalf])           \n\t"
        "jal x0,Exit            \n\t"


        "Exit:                  \n\t"

        :[head] "+r"(head),[firstHalf] "+r"(firstHalf),[secondHalf] "+r"(secondHalf)
        :
        :"x5","x6","x7","x28","x29","x30","x31"
    );



    printf("head for next round: %p\n",*firstHalf);
    printf("--------------------------\n");
}

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;
    
    asm volatile(
        /*
        Block B (mergeSortedList), which merges two sorted lists into one
        */

        // I drafted it in README.
        "addi x5,%[a],0         \n\t" //a = x6
        "addi x6,%[b],0         \n\t" //b = x6
        "bne x5,x0,jump1        \n\t"
        "addi %[result],x5,0    \n\t"
        "jal x0,Exita           \n\t"
        "jump1:                 \n\t"
        "bne x6,x0,jump2        \n\t"
        "addi %[result],x6,0    \n\t"
        "jal x0,Exita           \n\t"
        "jump2:                 \n\t"

        "ld x28,0(x5)           \n\t" //rs1(a) = x28
        "ld x29,0(x6)           \n\t" //rs2(b) = x29
        "blt x29,x28,elseone    \n\t"
        "addi %[result],x5,0    \n\t"
        "addi x7,x5,0           \n\t" //tail = x7
        "ld x30,8(x5)           \n\t" //a.next = x30
        "addi x5,x30,0          \n\t"
        "jal x0,finishone       \n\t"
        "elseone:               \n\t"
        "addi %[result],x6,0    \n\t"
        "addi x7,x6,0           \n\t" 
        "ld x30,8(x6)           \n\t"
        "addi x6,x30,0          \n\t"
        "finishone:             \n\t"

        "loopa:                 \n\t"
        "beq x5,x0,outa         \n\t"
        "beq x6,x0,outa         \n\t"
        "ld x28,0(x5)           \n\t"
        "ld x29,0(x6)           \n\t"
        "blt x29,x28,elsetwo    \n\t"
        "sd x5,8(x7)            \n\t"
        "addi x7,x5,0           \n\t"
        "ld x30,8(x5)           \n\t"
        "addi x5,x30,0          \n\t"
        "jal x0,finishtwo       \n\t"
        "elsetwo:               \n\t"
        "sd x6,8(x7)            \n\t"
        "addi x7,x6,0           \n\t"
        "ld x30,8(x6)           \n\t"
        "addi x6,x30,0          \n\t"
        "finishtwo:             \n\t"
        "jal x0,loopa           \n\t"
        "outa:                  \n\t"

        "bne x5,x0,elsethree    \n\t"
        "sd x6,8(x7)            \n\t"
        "jal finishthree        \n\t"
        "elsethree:             \n\t"
        "sd x5,8(x7)            \n\t"
        "finishthree:           \n\t"
        "Exita:                 \n\t"

        :[a] "+r"(a),[b] "+r"(b),[result] "+r"(result),[tail] "+r"(tail)
        :
        :"x5","x6","x7","x28","x29","x30"
    );
    printf("----MergesortList----\n");
    Node *cur = result;
    while(cur){
        printf("|%d", cur->data);
        printf("/%p|\n",cur->next);
        cur = cur->next;
    }
    printf("-----------------------\n");
    
    return result;
    
    return result;
}

int counter = 0;

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node
    Node *firstHalf, *secondHalf;

    counter ++;
    if(counter == 50){
        printf("too much\n");
        return 0;
    }
    printf("iteration: %d\n",counter);
    printf("Now enter splitList\n");
    Node *cur = head;
    cur = head;
    printf("Data:\n");
    while(cur){
        printf("%d\n", cur->data);
        cur = cur->next;
    }
    printf("*******\n");


    splitList(head, &firstHalf,
              &secondHalf); // Split the list into two sublists

    firstHalf = mergeSort(firstHalf);   // Recursively sort the left half
    secondHalf = mergeSort(secondHalf); // Recursively sort the right half

    return mergeSortedLists(firstHalf, secondHalf); // Merge the sorted sublists
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int list_size;
    fscanf(input, "%d", &list_size);
    Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
    Node *cur = head;
    for (int i = 0; i < list_size; i++) {
        fscanf(input, "%d", &(cur->data));
        if (i + 1 < list_size)
            cur->next = (Node *)malloc(sizeof(Node));
        cur = cur->next;
    }
    fclose(input);

    // Linked list sort
    head = mergeSort(head);

    cur = head;
    while (cur) {
        printf("%d ", cur->data);
        //cur = cur->next;
    asm volatile(
        /*
        Block C (Move to the next node), which updates the pointer to
        traverse the linked list
        */
        "ld x5,8(%[cur])\n\t"
        "add %[cur],x0,x5\n\t"
        :[cur] "+r"(cur)
        :
        :"x5","x6"
    );
    }
    printf("\n");
    return 0;
}