#include "list.h"
#include <stdio.h>
#include <stdlib.h> 

List* create_list()
{
    List* list = (List*)malloc(sizeof(List));
    if(list == NULL){
        printf("Error when taking memory for list");
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

//dodajemo cvor od nazad
void add_node_to_back(List* list,void* data)
{
    Node* node = (Node*)malloc(sizeof(Node));
    if(node == NULL){
        printf("Error when taking memory for new node");
        return;
    }
    (*node).data = data;
    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
        node->next = NULL;
        node->previus = NULL;
    }
    else {
        list->tail->next = node;
        node->previus = list->tail;
        node->next = NULL;
        list->tail = node;
    }
    list->size++;
}

void add_node_to_front(List *list,void *data)
{
    Node *node = (Node*)malloc(sizeof(Node));
    if(node == NULL)
    {
        printf("Error when taking memory for node");
        return;
    }
    node->data = data;
    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
        node->next = NULL;
        node->previus = NULL;
    }
    else {
        list->head->previus = node;
        node->next = list->head;
        node->previus = NULL;
        list->head = node;
    }
    list->size++;
}
void list_remove_node(List* list,void* data,CompareFunc cmp)
{
    Node* curr = list->head;
    while(curr != NULL)
    {
        Node* next = curr->next;
        if(cmp(curr->data,data) == 0)
        {
            if(curr == list->head)
            {
                list->head = curr->next;
                if(curr->next)
                    (curr->next)->previus= NULL;
                else
                    list->tail = NULL;
            }
            else if(curr == list->tail)
            {
                list->tail = curr->previus;
                if(curr->previus)
                    (list->tail)->next = NULL;

            }
            else 
            {
                Node* prev = curr->previus;
                prev->next = curr->next;

                Node* next = curr->next;
                next->previus = prev;
            }
            

            free(curr);
            list->size--;
        }
        curr = next;
    }
}
void list_remove_node_at(List* list,Node* node)
{
    if(list->head == node)
    {
        list->head = node->next;
        if(node->next)
            (node->next)->previus = NULL;
        else
            list->tail = NULL;
        
        
    }
    else if(list->tail == node)
    {
        list->tail = node->previus;
        if(node->previus)
            node->previus->next = NULL;
        
        
    }
    else
    {
        (node->previus)->next = node->next;
        (node->next)->previus = node->previus;
        
        
    }
    free(node);
    list->size--;
}
void free_list(List *list)
{
    Node *next = NULL;
    Node *current = list->head;
    while(current)
    {
        Node *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void print_list(List *list)
{
    if(list->size==0)
    {
        printf("\nList is empty");
        return;
    }
    Node *temp = list->head;
    while (temp != NULL) {
        printf("\n%d",*((int*)(temp->data)));
        temp = temp->next;
    }
}