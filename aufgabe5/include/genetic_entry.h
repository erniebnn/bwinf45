#ifndef __GENETIC_ENTRY_H__
#define __GENETIC_ENTRY_H__

#include "typedefs.h"

typedef u32 GeneticEntryID;

typedef struct _GeneticEntry {
    u32 unique_identifier;
    char* name;

    GeneticEntryID mother;
    GeneticEntryID father;
    
    u32 children_len;
    GeneticEntryID* children; 
} GeneticEntry;

typedef struct _GeneticArena {
    u32 entries_len;
    u32 entries_cap;
    GeneticEntry* entries;
} GeneticArena;

GeneticArena geneticarena_from_file(const char* p_path);
GeneticEntry* geneticarena_get(GeneticArena* p_arena, GeneticEntryID p_id);
GeneticEntryID geneticarena_getid(GeneticArena* p_arena, GeneticEntry* p_entry);
GeneticEntry* geneticarena_find(GeneticArena* p_arena, u32 p_uid);

#endif // #ifndef __GENETIC_ENTRY_H__