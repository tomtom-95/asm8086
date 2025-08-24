#ifndef HASHMAP_C
#define HASHMAP_C

#include "utils.c"
#include "arena.c"

#include "hashmap.h"

u64
hash_integer(u64 data)
{
    return (uint64_t)data * 2654435761u;
}

HashMap
hashmap_init(Arena *arena, u64 bucket_count)
{
    return (HashMap){ 
        .index_array = arena_push(arena, sizeof(u64) * bucket_count),
        .collision_array = arena_push(arena, MegaByte(1)),
        .offset = 0, .first_free = 0, .bucket_count = bucket_count
    };
}

u64
hashmap_find(HashMap *hash_map, u64 data)
{
    u64 bucket = hash_integer(data) % hash_map->bucket_count;

    u64 idx = hash_map->index_array[bucket];
    while (idx != 0) {
        if (hash_map->collision_array[idx].data == data) {
            return idx;
        }
        else {
            idx = hash_map->collision_array[idx].next;
        }
    }

    return idx;
}

void
hashmap_insert(HashMap *hash_map, u64 data)
{
    // make sure data is not already in the map
    assert(hashmap_find(hash_map, data) == 0);

    u64 bucket = hash_integer(data) % hash_map->bucket_count;

    u64 free_slot = hash_map->first_free;
    if (free_slot) {
        hash_map->first_free = hash_map->collision_array[hash_map->first_free].next;
    }
    else {
        free_slot = ++hash_map->offset;
    }

    u64 idx = hash_map->index_array[bucket];

    hash_map->index_array[bucket] = free_slot;
    hash_map->collision_array[free_slot].data = data;
    hash_map->collision_array[free_slot].next = idx;
}

void
hashmap_pop(HashMap *hash_map, u64 data)
{
    u64 bucket = hash_integer(data) % hash_map->bucket_count;

    u64 *idx_ptr = &hash_map->index_array[bucket];
    while (*idx_ptr != 0) {
        u64 idx = *idx_ptr;
        if (hash_map->collision_array[idx].data == data) {
            *idx_ptr = hash_map->collision_array[idx].next;

            hash_map->collision_array[idx].next = hash_map->first_free;
            hash_map->first_free = idx;

            return;
        }
        else {
            idx_ptr = &hash_map->collision_array[idx].next;
        }
    }
}

#endif // HASHMAP_C
