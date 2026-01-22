#include <stdlib.h>
#include "stdio.h"
#include "../strukture/hashmapa/hashmap.h"

//valgrind --leak-check=full ./a.out
//valgrind --tool=helgrind ./a.out


#define NUM_WRITERS 3
#define NUM_READERS 3
#define OPS_PER_THREAD 20000

Hashmap* hm;

void* multi_writer_thread(void* arg) {
    int id = *(int*)arg;
    int* values[OPS_PER_THREAD];
    for(int i = 0; i < OPS_PER_THREAD; i++) {
        values[i] = (int*)malloc(sizeof(int));
        if (values[i] == NULL) continue;
        
        // Svaki pisac dobija svoj opseg ključeva
        int key = (id * OPS_PER_THREAD) + i; 
        *values[i] = key * 2;
        hash_map_put(hm, key, values[i]);
    }
    for(int i = 0;i < OPS_PER_THREAD;i++)
        free(values[i]);
    return NULL;
}

void* multi_reader_thread(void* arg) {
    int found = 0;
    int total_keys = NUM_WRITERS * OPS_PER_THREAD;
    for(int i = 0; i < total_keys; i++) {
        int* val = (int*)get_value(hm, i);
        if(val) found++;
    }
    //printf("Reader: Pretraga zavrsena. Pronadjeno elemenata: %d\n", found);
    return NULL;
}

int main() {
    hm = init_hashmap(100);

    pthread_t writers[NUM_WRITERS];
    pthread_t readers[NUM_READERS];
    int writer_ids[NUM_WRITERS];

    printf("Pokrecem %d pisca i %d citaca...\n", NUM_WRITERS, NUM_READERS);

    for(int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i;
        pthread_create(&writers[i], NULL, multi_writer_thread, &writer_ids[i]);
    }

    for(int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, multi_reader_thread, NULL);
    }

    for(int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }
    for(int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    printf("Sve niti su zavrsile. Ukupno elemenata u mapi: %d\n", hm->size);

    printf("Ciscenje memorije...\n");
    free_hash_map(hm);

    printf("Test uspesno zavrsen.\n");
    return 0;
}