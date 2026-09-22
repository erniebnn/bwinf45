#pragma once
#include "typedefs.h"
#include "image.h"

typedef struct _EdgeMap {
    u32 width;
    u32 height;
    u8* buffer;
} EdgeMap;

EdgeMap edgemap_from(Image* p_image);
void edgemap_free(EdgeMap* p_edgemap);

void edgemap_resize(EdgeMap* p_edgemap, u32 p_width, u32 p_height);
void edgemap_threshold(EdgeMap* p_edgemap, u8 p_threshold);

double edgemap_compare(EdgeMap* p_edgemap0, EdgeMap* p_edgemap1);

void edgemap_dump(EdgeMap* p_edgemap, const char* p_path);