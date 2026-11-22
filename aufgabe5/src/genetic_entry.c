#include "genetic_entry.h"
#include <stdlib.h>
#include "panic.h"

GeneticArena geneticarena_from_file(const char* p_path) {
    
}

GeneticEntry* geneticarena_get(GeneticArena* p_arena, GeneticEntryID p_id) {
    return &p_arena->entries[p_id];
}

GeneticEntryID geneticarena_getid(GeneticArena* p_arena, GeneticEntry* p_entry) {
    u32 offset = (u32) (p_entry - p_arena->entries) / sizeof(GeneticEntry);
    iassert(p_arena->entries <= p_entry && offset < p_arena->entries_len, "p_entry is not contained in p_arena!");
    return (GeneticEntryID) offset;
}

GeneticEntry* geneticarena_find(GeneticArena* p_arena, u32 p_uid) {
    for (u32 i = 0; i < p_arena->entries_len; i++) {
        if (p_arena->entries[i].unique_identifier == p_uid) {
            return &p_arena->entries[i];
        }
    }
    return NULL;
}
