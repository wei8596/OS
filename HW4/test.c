#include <stdio.h>
#include "mm.h"

typedef struct node {
    int *ptr;
    int size;
    struct node* pNext;
}Node;

void enqueue(Node **front, Node **rear, int *p, int num);
int* dequeue(Node **front, Node **rear);
void print(Node *current);

int main(void) {
    int select, num, i, *newptr = NULL;
    Node *front = NULL, *rear = NULL;

    printf("1.malloc\n"
           "2.calloc\n"
           "3.realloc\n"
           "4.exit\n\n");
    scanf(" %d", &select);

    while(select != 4) {
        switch(select) {
            case 1:
                printf("array_size: ");
                scanf(" %d", &num);
                newptr = (int*)mymalloc(sizeof(int)*num);
                printf("reset element value?(1.yes 2.no): ");
				int reset;
                scanf(" %d", &reset);
                if(reset == 1) {
                    for(i = 0; i < num; ++i) {
                        printf("array[%d]: ", i);
                        scanf(" %d", &newptr[i]);
                    }
                }
                printf("array_element: ");
                for(i = 0; i < num; ++i) {
                    printf("%d ", newptr[i]);
                }
                enqueue(&front, &rear, newptr, num);
                break;
            case 2:
                printf("array_size: ");
                scanf(" %d", &num);
                newptr = (int*)mycalloc(sizeof(int), num);
                printf("array_element: ");
                for(i = 0; i < num; ++i) {
                    printf("%d ", newptr[i]);
                }
                enqueue(&front, &rear, newptr, num);
                break;
            case 3:
                if(!front) {
                    printf("The queue is Empty!\n");
                    break;
                }
                else {
                    newptr = dequeue(&front, &rear);
                }
                printf("array_size: ");
                scanf(" %d", &num);
                newptr = myrealloc(newptr, sizeof(int)*num);
                printf("array_element: ");
                for(i = 0; i < num; ++i) {
                    printf("%d ", newptr[i]);
                }
                enqueue(&front, &rear, newptr, num);
                break;
        }
        printf("\n\n");
        print(front);

        printf("\n1.malloc\n"
               "2.calloc\n"
               "3.realloc\n"
               "4.exit\n: ");
        scanf(" %d", &select);
    }

    while(front) {        
        int *tempPtr = dequeue(&front, &rear);
        myfree(tempPtr);
    }

    return 0;
}

void enqueue(Node **front, Node **rear, int *p, int num) {
    Node* temp = (Node*)malloc(sizeof(Node));
    
	if (temp) {
        temp->ptr=p;
		temp->size=num;
		temp->pNext=NULL;
        if (*front == NULL && *rear == NULL){
            *front = temp;
        }
        else {
            (*rear)->pNext = temp;     
        }
		*rear = temp;
    }
	else {
        printf("Enqueue Error!\n");
    }
}

int* dequeue(Node **front, Node **rear) {
    Node* temp = *front;
    int *p = (*front)->ptr;

	if (*front == *rear) {
		*front = *rear = NULL;
	}
	else {
		*front = (*front)->pNext;
	}

    free(temp);
    return p;
}

void print(Node *pHead) {
	Node* pCurrent = pHead;
	
    if(pCurrent) {
        while(pCurrent) {
            printf("address: %p\tsize: %d\n", pCurrent->ptr, pCurrent->size*(int)sizeof(int));
            pCurrent=pCurrent->pNext;
        }        
    }
    else {
		printf("Empty!");
    }
	printf("\n");
}
