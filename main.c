#include <stdlib.h>
#include "stdio.h"
#include "strukture/hashmapa/hashmap.h"

int main()
{   
    Hashmap* hm = init_hashmap(0);
    int* value = (int*)malloc(sizeof(int));
    *value = 5;
    hash_map_put(hm, 5, value);
    int vr = *(int*)(get_value(hm, 5));
    hashmap_remove(hm, 5);
    print_hash_map(hm);
    printf("%d",vr);
    return 0;
}