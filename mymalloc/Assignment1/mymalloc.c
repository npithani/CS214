//
//  mymalloc.c
//  mymalloc
//
//  Created by Neha Srisatya Pithani on 2/13/18.
//  Copyright © 2018 Neha Srisatya Pithani. All rights reserved.
//

#include <stdio.h>
#include "mymalloc.h"
#define SECURE 3343  //security code


typedef struct LL{           //creates a doubly linked list the metadata linked list
    
    struct LL* next;        //points to the next node in the linked list
    struct LL* prev;        //points to the previous node in the liked list
    unsigned int allocated:1;    //variable to determine if memory is allocated or not
    int size;                     //size of the memory of that node
    unsigned int secure:16;        //security code for that node
    
} Node;

static int firstmalloccalled = 0;    //determines if malloc is called or not
static char myblock[MEM_SIZE];    //creates an array of size 5000
static Node* head = (Node*)(&(myblock[0]));



void* mymalloc(int memory){
    
    if(memory > MEM_SIZE - sizeof(Node)){//checks if requested size is less than the available size + metadata
        
        printf("Error is calling mymalloc \n");
        return NULL;
        
    }
    
    int donemalloc = 0;
    
    //Creates node for first allocated portion
    if(!firstmalloccalled){
        
        head->size = MEM_SIZE - sizeof(Node);
        head->next = NULL;
        head->prev = NULL;
        head->allocated = 0;
        head->secure = SECURE;
        firstmalloccalled = 1;
        
    }
    
    int index = 0;
    
    Node *ptr = head;
    while(ptr != NULL){
        //Checkes if current node contains the required memory and is not allocated
        if(ptr->size >= memory && ptr->allocated == 0){
            
            break;
            
        }
        index = index + sizeof(Node) + ptr->size;
        //increments ptr pointer
        ptr = ptr->next;
        
    }
    
    int temp = ptr->size;
    ptr->size = memory;
    ptr->allocated = 1;
    //
    if(ptr->next == NULL){
        //Creates a new node for the allocation of the requested memory
        if(temp - memory > sizeof(Node)){
            
            ptr->next = (Node*)(&(myblock[index + sizeof(Node) + memory]));
            ptr->next->allocated = 0;
            ptr->secure = SECURE;
            ptr->next->size = temp - sizeof(Node) - memory;
            ptr->next->next = NULL;
            ptr->next->prev = ptr;
            
        }else{
            //update the size of current node
            ptr->size = ptr->size + (temp - memory);
            
        }
        //malloc is done
        donemalloc = 1;
        
        
    }else{
        //creates a new node with a specific security code
        if(temp - memory > sizeof(Node)){
            
            Node* temporaryNode = ptr->next;
            ptr->next = (Node*)(&(myblock[index + sizeof(Node) + memory]));
            ptr->secure = SECURE;
            ptr->next->allocated = 0;
            ptr->next->size = temp - sizeof(Node) - memory;
            ptr->next->next = temporaryNode;
            ptr->next->prev = ptr;
            temporaryNode->prev = ptr->next;
            
        }else{
            //update the size of the current node
            ptr->size = ptr->size + (temp - memory);
            
        }
        // malloc is done
        donemalloc = 1;
        
        
    }
    
    //if malloc does not ouput one, then outputs error
    if(donemalloc == 0){
        
        printf("Error in calling mymalloc");
        return NULL;
        
    }
    
    
    
    
    
    
    return (char*)ptr + sizeof(Node);
    
}






//Free method
void myfree(void* input){
    
    
    Node* ptr = (Node*)(input - sizeof(Node));
    //Error if pointer is not allocated or has wrong security code
    if(ptr->secure != SECURE || ptr->allocated == 0){
        
        printf("ERROR: cannot free pointer \n");
        return;
        
    }
    
    //increments pointer to look for the requested pointer
    if(ptr->next != NULL && ptr->prev == NULL){
        
        Node* temp = ptr->next;
        //Frees current node if it is meant to be freed
        if(temp->allocated == 0){
            
            ptr->size = ptr->next->size + ptr->size + sizeof(Node);
            ptr->allocated = 0;
            ptr->next = ptr->next->next;
            temp->prev = NULL;
            
            if(ptr->next != NULL){
                
                ptr->next->prev = ptr;
                
            }
            
        }else{
            
            ptr->allocated = 0;
            
        }
        
    }else if(ptr->next == NULL && ptr->prev != NULL){
        
        Node* temp = ptr->prev;
        
        if(temp->allocated == 0){
            //if the previous is also free then both nodes are combined in the array
            if(temp->prev == NULL){
                
                temp->size = temp->size + ptr->size + sizeof(Node);
                temp->next = ptr->next;
                ptr->prev = ptr;
                
                if(temp->next != NULL){
                    
                    temp->next->prev = temp;
                    
                }
                
            }else{
                
                temp->size = temp->size + sizeof(Node) + ptr->size;
                temp->next = NULL;
                
            }
            
        }else{
            
            ptr->allocated = 0;
            
        }
        
    }else if(ptr->next != NULL && ptr->prev != NULL){
        //combines two unallocated blocks to reduce memory fragmentation
        if(ptr->next->allocated == 0 && ptr->prev->allocated == 0){
            
            ptr = ptr->prev;
            ptr->size = ptr->size + sizeof(Node) + ptr->next->size + sizeof(Node) + ptr->next->next->size;
            
            
            if(ptr->next->next->next != NULL){
                
                ptr->next = ptr->next->next->next;
                ptr->next->next->next->prev = ptr;
                
            }else{
                
                ptr->next = NULL;
                
            }
            
        }else if(ptr->prev->allocated != 0 && ptr->next->allocated == 0){
            
            
            ptr->size = ptr->size + sizeof(Node) + ptr->next->size;
            
            if(ptr->next->next != NULL){
                
                ptr->next = ptr->next->next;
                ptr->next->prev = ptr;
                ptr->allocated = 0;
                
            }else{
                
                ptr->next = NULL;
                ptr->allocated = 0;
                
            }
            
            
        }else if(ptr->prev->allocated == 0 && ptr->next->allocated != 0){
            
            ptr = ptr->prev;
            ptr->size = ptr->size + sizeof(Node) + ptr->next->size;
            ptr->next = ptr->next->next;
            ptr->next->prev = ptr;
            
            
            
        }else{
            
            ptr->allocated = 0;
            
        }
        
        
    }else if(ptr->next == NULL && ptr->prev == NULL){
        
        ptr->allocated = 0;
        
    }
    
    
}
