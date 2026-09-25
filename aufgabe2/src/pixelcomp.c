#include "pixelcomp.h"
#include "image.h"
#include <math.h>
#include "panic.h"

double image_pixel_compare(Image* p_image0, Image* p_image1) {
    iassert(p_image0->width == p_image1->width && p_image0->height == p_image1->height, "Images to compare must be of the same size!");
    iassert(p_image0->kind == p_image1->kind, "Images to compare must be of the same kind!");
    const u64 max_px_dist = p_image0->kind == IMGK_GRAY ? 255 : 442; // ceil(sqrt(3*255^2))
    u64 max_dist = 0;
    u64 total_dist = 0;
    for (u32 y = 0; y < p_image0->height; y++) {
        for (u32 x = 0; x < p_image0->width; x++) {
            u32 pixel_idx = y * p_image0->width + x;
            if (p_image0->kind == IMGK_RGBA) {
                u32 dist_r = abs((i32) p_image0->buffer[4 * pixel_idx + 0] - (i32) p_image1->buffer[4 * pixel_idx + 0]);
                u32 dist_g = abs((i32) p_image0->buffer[4 * pixel_idx + 1] - (i32) p_image1->buffer[4 * pixel_idx + 1]);
                u32 dist_b = abs((i32) p_image0->buffer[4 * pixel_idx + 2] - (i32) p_image1->buffer[4 * pixel_idx + 2]);
                u32 img0_a = p_image0->buffer[4 * pixel_idx + 3];
                u32 img1_a = p_image1->buffer[4 * pixel_idx + 3];
                u32 cur_dist = (u32) sqrt((double) (dist_r * dist_r + dist_g * dist_g + dist_b * dist_b));
                if (img0_a != 0 || img1_a != 0) { max_dist += max_px_dist; }
                if (img0_a != 0 && img1_a != 0) { total_dist += cur_dist; }
            }
            else if (p_image0->kind == IMGK_GRAY) {
                u32 dist = abs((i32) p_image0->buffer[2 * pixel_idx + 0] - (i32) p_image1->buffer[2 * pixel_idx + 0]);
                u32 img0_a = p_image0->buffer[2 * pixel_idx + 1];
                u32 img1_a = p_image1->buffer[2 * pixel_idx + 1];
                if (img0_a != 0 || img1_a != 0) { max_dist += max_px_dist; }
                if (img0_a != 0 && img1_a != 0) { total_dist += dist; }
            }
            else {
                panic("Unknown enum variant!");
            }
        }
    }
    return 1.0 - ((double) total_dist / (double) max_dist);
}