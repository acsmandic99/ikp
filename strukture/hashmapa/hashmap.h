#ifndef HASHMAP
#define HASHMAP

#include "../lista/list.h" 
#include <stdlib.h>
#include <pthread.h>


typedef struct Hashmap{
    int capacity;
    int size;
    List** data;
    pthread_mutex_t lock;
}Hashmap;

typedef struct Entry{
    int key;
    void* data;
}Entry;

Hashmap* init_hashmap(int capacity);
void hash_map_put(Hashmap* hashm,int key,void *value);
void* get_value(Hashmap* hm,int key);
void print_hash_map(Hashmap* hm);
void free_hash_map(Hashmap* hm, int free_all);
void* hashmap_remove(Hashmap* hashm,int key);
void hashmap_remove_and_data(Hashmap* hm,int key);
//dokumentacija-> sta unaprediti:
//1.    da se ne koristi globalni lock za celu hm nego
//      samo na listu u kojoj se pise 
//2.    u resize-u umesto da brisem node-ove pa pravim nove
//      za new_data mogao bi da ih samo da ih prevezem
#endif