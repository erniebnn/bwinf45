#include "pixelcomp.h"
#include "panic.h"

double image_pixel_compare(Image* p_image0, Image* p_image1) {
    assert(p_image0->width == p_image1->width && p_image0->height == p_image1->height, "Images to compare must be of the same size!");
    assert(p_image0->kind == p_image1->kind, "Images to compare must be of the same kind!");
    const u32 max_dist = 442; // ceil(sqrt(3*255^2))
    for (u32 y = 0; y < p_image0->height; y++) {
        for (u32 x = 0; x < p_image0->width; x++) {
            u32 pixel_idx = y * p_image0->width + x;
            u32 dist_r = abs((i8) )
        }
    }
}