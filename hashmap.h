#ifndef HASHMAP_H
#define HASHMAP_H

#include "utils.c"
#include "arena.h"

typedef struct Element Element;
struct Element {
    u64 data;
    u64 next;
};

typedef struct HashMap HashMap;
struct HashMap {
    u64 *index_array;
    Element *collision_array;
    u64 offset;
    u64 first_free;
    u64 bucket_count;
};


u64     hash_integer(u64 data);
HashMap hashmap_init(Arena *arena, u64 bucket_count);
u64     hashmap_find(HashMap *hash_map, u64 data);
void    hashmap_insert(HashMap *hash_map, u64 data);
void    hashmap_pop(HashMap *hash_map, u64 data);

#endif // HASHMAP_H
