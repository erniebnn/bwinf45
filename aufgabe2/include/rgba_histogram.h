#pragma once
#include "image.h"

typedef struct _RGBAHistogram {
    u64* buffer;
    u64 mass;
    u32 size;
} RGBAHistogram;

RGBAHistogram histogram_from(Image* p_image, u32 p_size);
void histogram_free(RGBAHistogram* p_hist);
double histogram_compare(RGBAHistogram* p_hist0, RGBAHistogram* p_hist1);

struct _IO_FILE;
void histogram_dump(RGBAHistogram* p_hist, struct _IO_FILE* p_output);