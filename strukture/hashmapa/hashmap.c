#include "hashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include "../lista/list.h"
#include <pthread.h>

#define MIN_CAPACITY 11
#define MAX_LOAD_FACTOR 0.75
#define MIN_LOAD_FACTOR 0.25
int hash(int x);
int next_prime(int num);
void resize(Hashmap* hashm,int new_capacity);

//Ako je kapacitet 0,uzimamo podrazumevanu vrednost od 11
Hashmap* init_hashmap(int capacity)
{
    Hashmap *hm = (Hashmap*)malloc(sizeof(Hashmap));
    if(hm == NULL)
    {
        printf("\nError ocured when taking memory for hashmap");
        return NULL;
//Ako je kapacitet 0,uzimamo podrazumevanu vrednost od 11
    }
    if(capacity <= 0)
        hm->capacity = 11;
    else
        hm->capacity = capacity;
    hm->size = 0;
    hm->data = (List**)malloc(hm->capacity*sizeof(List*));
    if(hm->data == NULL)
    {
        free(hm);
        printf("\nError ocured when taking memory for data");
        return NULL;
    }
    for(int i = 0;i<hm->capacity;i++)
    {
        hm->data[i] = create_list();
        if(hm->data[i] == NULL)
        {
            printf("Error ocurred while taking memory for list in hashmap");
            for (int j = 0; j < i; ++j) 
                free_list(hm->data[j]);
            free(hm->data);
            free(hm);
            return NULL;
        }
    }
    pthread_rwlock_init(&hm->lock, NULL);
    return hm;
}
//Dodaje novi elemnt ili update-uje postojeci tim kljucem
void hash_map_put(Hashmap* hm,int key,void *value)
{
    pthread_rwlock_wrlock(&hm->lock);
    int index = (unsigned int)hash(key) % hm->capacity;
    List* lista = hm->data[index];
    Node* curr = lista->head;
    while(curr != NULL)
    {
        Entry* entry = curr->data;
        if(entry->key == key)
        {
            free(entry->data);
            entry->data = value;//update
            pthread_rwlock_unlock(&hm->lock);
            return;
        }
        curr = curr->next;
    }
    //dodavanje novog
    Entry* new_value = (Entry*)malloc(sizeof(Entry));
    if(new_value == NULL)
    {
        printf("\nError ocured when taking memory for hashmap");
        pthread_rwlock_unlock(&hm->lock);
        return;
    }
    new_value->key = key;
    new_value->data = value;
    add_node_to_front(hm->data[index],new_value);
    hm->size++;
    if((float)hm->size/hm->capacity > MAX_LOAD_FACTOR)
    {
        int old_capacity = hm->capacity;
        int new_capacity= next_prime(old_capacity * 2);
        resize(hm,new_capacity);
    }
    pthread_rwlock_unlock(&hm->lock);
}
void resize(Hashmap* hm,int new_capacity)
{
    if (new_capacity < MIN_CAPACITY) 
        new_capacity = MIN_CAPACITY;

    int old_capacity = hm->capacity;

    List** new_data = (List**)malloc(new_capacity*sizeof(List*));
    if(new_data == NULL)
    {
        printf("\nError ocured while doing resize of hashmap");
        return;
    }
    for(int i =0;i<new_capacity;i++)
    {
        new_data[i] = create_list();
        if(new_data[i] == NULL)
        {
            printf("Error ocurred while taking memory for list in hashmap");
                for (int j = 0; j < i; ++j) 
                    free_list(new_data[j]);
                free(new_data);
                return;
        }
    }
    List** old_data = hm->data;
    hm->data = new_data;
    hm->capacity = new_capacity;

    for(int i = 0;i<old_capacity;i++)
    {
        Node* curr = old_data[i]->head;
        while (curr)
        {
            Entry* value = curr->data;
            int index = (unsigned int)hash(value->key) % new_capacity;
            add_node_to_front(new_data[index], value); 
            curr = curr->next;
        }
        
    }
    for(int i = 0;i<old_capacity;i++)
    {
        free_list(old_data[i]);
    }
    free(old_data);
}
void* get_value(Hashmap* hm,int key)
{
    pthread_rwlock_rdlock(&hm->lock);
    int index = (unsigned int)hash(key) % hm->capacity;
    Node *curr = hm->data[index]->head;
    while(curr)
    {
        Entry* data = curr->data;
        if(data->key == key)
        {
            pthread_rwlock_unlock(&hm->lock);
            return data->data;
        }
        curr = curr->next;
    }
    pthread_rwlock_unlock(&hm->lock);
    return NULL;
}

//hashmapa samo izbaci iz kolekcije
//data deo ostaje u memoriji
void* hashmap_remove(Hashmap* hm,int key)
{
    pthread_rwlock_wrlock(&hm->lock);
    int index = (unsigned int)hash(key) % hm->capacity;
    Node* curr = hm->data[index]->head;
    while(curr)
    {
        if(((Entry*)curr->data)->key == key)
        {
            void* data = ((Entry*)curr->data)->data;
            free(curr->data);
            list_remove_node_at(hm->data[index],curr);
            hm->size--;
            if((float)hm->size/hm->capacity < MIN_LOAD_FACTOR)
            {
                int old_capacity = hm->capacity;
                int new_capacity= next_prime(old_capacity / 2);
                resize(hm, new_capacity);
            }
            pthread_rwlock_unlock(&hm->lock);
            return data;
        }
        curr = curr->next;
    }
    pthread_rwlock_unlock(&hm->lock);
}

//samo sa int-radi testiranja
void print_hash_map(Hashmap* hm)
{
    pthread_rwlock_rdlock(&hm->lock);
    if(hm->size == 0)
    {
        printf("\nHashmap is empty");
    }
    printf("\nSize:%d, Capacity:%d",hm->size,hm->capacity);
    for(int i = 0;i<hm->capacity;i++)
    {
        List* lista = ((List*)hm->data[i]);
        if(lista->size > 0)
        {
            Node* curr = lista->head;
            while(curr != NULL) 
            {
                Entry* data = ((Entry*)curr->data);
                printf("\nKey: %d Value:%d",data->key,*((int*)data->data));
                curr = curr->next;
            }
        }
    }
    pthread_rwlock_unlock(&hm->lock);
}
//Entry->Data se ne oslobadja o tome mora da misli njihov vlasnik
//moraju se zaustaviti svi thredovi koji rade pa tek onda pozvati
//free(map)
void free_hash_map(Hashmap* hm)
{
    
    for(int i =0;i<hm->capacity;i++)
    {
        List* lista = hm->data[i];
        Node* curr = lista->head;
        while(curr != NULL)
        {
            free(curr->data);
            curr = curr->next;
        }
        free_list(lista);
    }
    pthread_rwlock_destroy(&hm->lock);
    free(hm->data);
    free(hm);
}
// Source - https://stackoverflow.com/a
// Posted by Thomas Mueller, modified by community. See post 'Timeline' for change history
// Retrieved 2026-01-20, License - CC BY-SA 4.0
int hash(int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = (x >> 16) ^ x;
    return x;
}

int is_prime(int num)
{
    if(num < 2) return 0;
    if(num == 2) return 1;
    if(num % 2 == 0) return 0;
    
    //po teoriji brojeva dovoljno je 
    //da se ide do koren(num) da se vidi dal je prost
    for(int i = 3; i * i <= num; i += 2)
        if(num % i == 0)
            return 0;

    return 1;
}
int next_prime(int num)
{
    while(!is_prime(num))
        num++;
    return num;
}
