#include "edgemap.h"
#include "stb_image_resize2.h"
#include "stb_image_write.h"
#include "image.h"
#include "panic.h"
#include <stdlib.h>

static u32 pixel_diff(u8 p_px0g, u8 p_px0a, u8 p_px1g, u8 p_px1a) {
    if (p_px0a == 0 || p_px1a == 0) { return 0; }
    return abs((i32) p_px0g - (i32) p_px1g);
}

EdgeMap edgemap_from(Image* p_image) {
    Image image = !image_is_grayscale(p_image) ? image_grayscale(p_image) : *p_image;
    EdgeMap res;
    res.width = image.width;
    res.height = image.height;
    res.buffer = (u8*) calloc(res.width * res.height, sizeof(*res.buffer));
    iassert(res.buffer != NULL, "Failed to allocate memory!");
    for (u32 y = 0; y < image.height - 1; y++) {
        for (u32 x = 0; x < image.width - 1; x++) {
            u32 pixel_idx = y * image.width + x;
            u32 right_idx = y * image.width + (x + 1);
            u32 down_idx  = (y + 1) * image.width + x;
            u32 dx = pixel_diff(
                image.buffer[2 * pixel_idx + 0],
                image.buffer[2 * pixel_idx + 1],
                image.buffer[2 * right_idx + 0],
                image.buffer[2 * right_idx + 1]
            );
            u32 dy = pixel_diff(
                image.buffer[2 * pixel_idx + 0],
                image.buffer[2 * pixel_idx + 1],
                image.buffer[2 * down_idx  + 0],
                image.buffer[2 * down_idx  + 1]
            );
            res.buffer[pixel_idx] = (dx + dy) > 255 ? 255 : dx + dy;
        }
    }
    if (!image_is_grayscale(p_image)) { image_free(&image); }
    return res;
}

void edgemap_free(EdgeMap* p_edgemap) {
    free(p_edgemap->buffer);
}

void edgemap_resize(EdgeMap* p_edgemap, u32 p_width, u32 p_height) {
    u8* buf = (u8*) malloc(p_width * p_height * sizeof(*buf));
    stbir_resize_uint8_linear(p_edgemap->buffer, p_edgemap->width, p_edgemap->height, 0, buf, p_width, p_height, 0, STBIR_1CHANNEL);
    free(p_edgemap->buffer);
    p_edgemap->width = p_width;
    p_edgemap->height = p_height;
    p_edgemap->buffer = buf;
}

void edgemap_threshold(EdgeMap* p_edgemap, u8 p_threshold) {
    for (u32 y = 0; y < p_edgemap->height; y++) {
        for (u32 x = 0; x < p_edgemap->width; x++) {
            u32 pixel_idx = y * p_edgemap->width + x;
            p_edgemap->buffer[pixel_idx] = p_edgemap->buffer[pixel_idx] > p_threshold ? 255 : 0;
        }
    }
}

double edgemap_compare(EdgeMap* p_edgemap0, EdgeMap* p_edgemap1) {
    iassert(p_edgemap0->width == p_edgemap1->width && p_edgemap0->height == p_edgemap1->height, "Edgemap dimensions must match to compare!");
    u32 matching_ones = 0;
    u32 edgemap0_mass = 0, edgemap1_mass = 0;
    for (u32 y = 0; y < p_edgemap0->height; y++) {
        for (u32 x = 0; x < p_edgemap0->width; x++) {
            u32 pixel_idx = y * p_edgemap0->width + x;
            u32 edgpx0 = p_edgemap0->buffer[pixel_idx];
            u32 edgpx1 = p_edgemap1->buffer[pixel_idx];
            matching_ones += (edgpx0 == 255 && edgpx1 == 255);
            edgemap0_mass +=  edgpx0 == 255;
            edgemap1_mass +=  edgpx1 == 255;
        }
    }
    u32 full_mass = edgemap0_mass + edgemap1_mass;
    if (full_mass == 0) { return 0.0; }
    return 2.0 * (double) matching_ones / (double) full_mass;  
}

void edgemap_dump(EdgeMap* p_edgemap, const char* p_path) {
    int res = stbi_write_png(
        p_path,
        p_edgemap->width,
        p_edgemap->height,
        1,
        p_edgemap->buffer,
        0
    );
    iassert(res != 0, "Failed to write to file '%s'!", p_path);
}