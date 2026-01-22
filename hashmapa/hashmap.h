#ifndef HASHMAP
#define HASHMAP

#include "../lista/list.h" 
#include <stdlib.h>
#include <pthread.h>


typedef struct Hashmap{
    int capacity;
    int size;
    List** data;
    pthread_rwlock_t lock;
}Hashmap;

typedef struct Entry{
    int key;
    void* data;
}Entry;

Hashmap* init_hashmap(int capacity);
void hash_map_put(Hashmap* hashm,int key,void *value);
void* get_value(Hashmap* hm,int key);
void print_hash_map(Hashmap* hm);
void free_hash_map(Hashmap* hm);
void hashmap_remove(Hashmap* hashm,int key);


//dokumentacija-> sta unaprediti:
//1.    da se ne koristi globalni lock za celu hm nego
//      samo na listu u kojoj se pise 
//2.    u resize-u umesto da brisem node-ove pa pravim nove
//      za new_data mogao bi da ih samo da ih prevezem
#endif