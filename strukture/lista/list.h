#ifndef LIST
#define LIST

typedef struct Node
{
    void* data;
    struct Node* next;
    struct Node* previus;

}Node;

typedef struct List
{
    Node* head;
    Node* tail;
    int size;
}List;

typedef int (*CompareFunc)(void* a, void* b);


List* create_list();
void add_node_to_back(List* list,void* data);
void print_list(List *list);
void free_list(List *list);
void add_node_to_front(List *list,void *data);
void list_remove_node(List* list,void* data,CompareFunc cmp);
void list_remove_node_at(List* list,Node* node);
#endif 