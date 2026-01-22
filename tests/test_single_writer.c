#include <stdlib.h>
#include "stdio.h"
#include "../strukture/hashmapa/hashmap.h"

//valgrind --leak-check=full ./a.out
//valgrind --tool=helgrind ./a.out

#define NUM_READERS 5
#define OPERATIONS 50000

Hashmap* hm;

void* writer_thread(void* arg) {
    int* values[OPERATIONS];
    for(int i = 0; i < OPERATIONS; i++) {
        values[i] = malloc(sizeof(int));
        *values[i] = i * 10; // Neka vrednost
        hash_map_put(hm, i, values[i]);
    }
    //printf("Writer: Zavrsio upis %d elemenata.\n", OPERATIONS);
    for(int i = 0;i < OPERATIONS;i++)
        free(values[i]);
    return NULL;
}

// Reader nit: Više čitalaca koji stalno proveravaju nasumicne kljuceve
void* reader_thread(void* arg) {
    int id = *(int*)arg;
    int found = 0;
    for(int i = 0; i < OPERATIONS; i++) {
        int key = i; 
        int* val = (int*)get_value(hm, key);
        
        if(val) {
            int temp = *val; 
            (void)temp;
            found++;
        }
    }
    //printf("Reader %d: Zavrsio, pronasao %d/%d elemenata.\n", id, found, OPERATIONS);
    return NULL;
}

int main() {
    hm = init_hashmap(1000);

    pthread_t writer;
    pthread_t readers[NUM_READERS];
    int reader_ids[NUM_READERS];

    pthread_create(&writer, NULL, writer_thread, NULL);

    for(int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i;
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }

    pthread_join(writer, NULL);
    for(int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for(int i = 0; i < 100; i++) {
        hashmap_remove(hm, i);
    }
    
    free_hash_map(hm);
    return 0;
}