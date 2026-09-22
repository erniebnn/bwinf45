#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "panic.h"

static u32 image_buffer_size(Image* p_image) {
    u32 bytes_per_pixel;
    switch (p_image->kind) {
        case IMGK_RGBA:
            bytes_per_pixel = 4;
            break;
        case IMGK_GRAY:
            bytes_per_pixel = 2;
            break;
        default:
            panic("Unknown enum variant!");
    }
    return p_image->width * p_image->height * bytes_per_pixel;
}

Image image_create(u32 p_width, u32 p_height, ImageKind p_kind) {
    Image res;
    res.kind = p_kind;
    res.width = p_width;
    res.height = p_height;
    res.buffer = (u8*) malloc(image_buffer_size(&res));
    assert(res.buffer != NULL, "Failed to allocate memory!");
    return res;
}

Image image_load(const char* p_path) {
    Image res;
    u32 channels;
    res.buffer = stbi_load(p_path, (i32*) &res.width, (i32*) &res.height, (i32*) &channels, 0);
    assert(res.buffer != NULL, "Could not load image file '%s': %s", p_path, stbi_failure_reason());
    if (channels == 2) {
        res.kind = IMGK_GRAY;
    }
    else if (channels == 4) {
        res.kind = IMGK_RGBA;
    }
    else {
        panic("Can only load images with 2 or 4 channels, found %d!", channels);
    }
    return res;
}

void image_save(Image* p_image, const char* p_path) {
    u32 channels = (p_image->kind == IMGK_RGBA) ? 4 : 2;
    i32 res = stbi_write_png(
        p_path,
        p_image->width,
        p_image->height,
        channels,
        p_image->buffer,
        p_image->width * channels
    );
    assert(res == 1, "Failed to write image '%s'", p_path);
}

void image_free(Image* p_flag) {
    free(p_flag->buffer);
};

Image image_copy(Image* p_image) {
    u32 buffer_len = image_buffer_size(p_image);
    Image res;
    res.kind = p_image->kind;
    res.width = p_image->width;
    res.height = p_image->height;
    res.buffer = (u8*) malloc(buffer_len);
    assert(res.buffer != NULL, "Failed to allocate memory!");
    memcpy(res.buffer, p_image->buffer, buffer_len);
    return res;
}

Image image_grayscale(Image* p_image) {
    if (image_is_grayscale(p_image)) {
        return image_copy(p_image);
    }
    Image res = image_create(p_image->width, p_image->height, IMGK_GRAY);
    for (u32 y = 0; y < p_image->height; y++) {
        for (u32 x = 0; x < p_image->width; x++) {
            u32 pixel_idx = y * p_image->width + x;
            u32 px_r = p_image->buffer[4 * pixel_idx + 0];
            u32 px_g = p_image->buffer[4 * pixel_idx + 1];
            u32 px_b = p_image->buffer[4 * pixel_idx + 2];
            u32 px_a = p_image->buffer[4 * pixel_idx + 3];
            res.buffer[2 * pixel_idx + 0] = (77 * px_r + 150 * px_g + 29 * px_b) >> 8;
            res.buffer[2 * pixel_idx + 1] = px_a;
        }
    }
    return res;
}

bool image_is_grayscale(Image* p_image) {
    return p_image->kind == IMGK_GRAY;
}