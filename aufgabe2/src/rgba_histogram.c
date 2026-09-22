#include "rgba_histogram.h"
#include "image.h"
#include "panic.h"
#include <stdlib.h>
#include <stdio.h>

RGBAHistogram histogram_from(Image* p_image, u32 p_size) {
    assert(!image_is_grayscale(p_image), "Histogram can only be created from non-grayscale image!");
    RGBAHistogram res;
    res.size = p_size;
    res.mass = 0;
    res.buffer = (u64*) calloc(p_size * p_size * p_size, sizeof(*res.buffer));
    assert(res.buffer != NULL, "Failed to allocate memory!");
    for (u32 y = 0; y < p_image->height; y++) {
        for (u32 x = 0; x < p_image->width; x++) {
            u32 pixel_idx = y * p_image->width + x;
            u32 px_r = p_image->buffer[4 * pixel_idx + 0] / (256 / p_size);
            u32 px_g = p_image->buffer[4 * pixel_idx + 1] / (256 / p_size);
            u32 px_b = p_image->buffer[4 * pixel_idx + 2] / (256 / p_size);
            u32 px_a = p_image->buffer[4 * pixel_idx + 3] / 16    ;
            res.buffer[px_b * (p_size * p_size) + px_g * p_size + px_r] += px_a;
            res.mass += px_a;
        }
    }
    return res;
}

void histogram_free(RGBAHistogram* p_hist) {
    free(p_hist->buffer);
}

double histogram_compare(RGBAHistogram* p_hist0, RGBAHistogram* p_hist1) {
    assert(p_hist0->size == p_hist1->size, "Compared histograms must have equal size!");
    u64 overlap = 0;
    for (u32 i = 0; i < p_hist0->size * p_hist0->size * p_hist0->size; i++) {
        u64 a = p_hist0->buffer[i] * p_hist1->mass;
        u64 b = p_hist1->buffer[i] * p_hist0->mass;
        overlap += a < b ? a : b;
    }
    u64 fact = p_hist0->mass * p_hist1->mass;
    if (fact == 0) { return 0.0; }
    return (double) overlap / (double) fact;
}

void histogram_dump(RGBAHistogram* p_hist, FILE* p_output) {
    u32 size = p_hist->size;
    fprintf(p_output, "RED                        ");
    for (u32 i = 0; i < size; i++) {
        fprintf(p_output, "%02x ", 256 * i / size);
    }
    fprintf(p_output, "\n");
    for (u32 px_b = 0; px_b < size; px_b++) {
        for (u32 px_g = 0; px_g < size; px_g++) {
            fprintf(p_output, "BLUE: 0x%02x, Green: 0x%02x :: ", 256 * px_b / size, 256 * px_g / size);
            for (u32 px_r = 0; px_r < size; px_r++) {
                u32 pixel_idx = px_b * (size * size) + px_g * size + px_r;
                u64 pixel_val = p_hist->buffer[pixel_idx];
                if (pixel_val != 0) {
                    fprintf(p_output, "%02x ", (unsigned int)(pixel_val / (p_hist->mass / 256)));
                }
                else {
                    fprintf(p_output, "   ");
                }
            }
            fprintf(p_output, "\n");
        }
    }
    fprintf(p_output, "\n");
}