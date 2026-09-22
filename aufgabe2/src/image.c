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
        case IMGK_RGBA_REDUCED:
            bytes_per_pixel = 2;
            break;
        case IMGK_GRAY_REDUCED:
            bytes_per_pixel = 1;
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
    if (channels == 1) {
        res.kind = IMGK_GRAY;
    }
    else if (channels == 4) {
        res.kind = IMGK_RGBA;
    }
    else {
        panic("Can only load images with 1 or 4 channels, found %d!", channels);
    }
    return res;
}

void image_save(Image* p_image, const char* p_path) {
    Image image = image_is_reduced(p_image) ? image_extend(p_image) : *p_image;
    u32 channels = (image.kind == IMGK_RGBA) ? 4 : 2;
    i32 res = stbi_write_png(
        p_path,
        image.width,
        image.height,
        channels,
        image.buffer,
        image.width * channels
    );
    assert(res == 1, "Failed to write image '%s'", p_path);
    if (image_is_reduced(p_image)) { image_free(&image); };
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
    ImageKind kind = (p_image->kind == IMGK_RGBA) ? IMGK_GRAY : IMGK_GRAY_REDUCED;
    Image res = image_create(p_image->width, p_image->height, kind);
    if (kind == IMGK_GRAY) {
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
    }
    else {
        for (u32 y = 0; y < p_image->height; y++) {
            for (u32 x = 0; x < p_image->width; x++) {
                u32 pixel_idx = y * p_image->width + x;
                u32 px_r = p_image->buffer[2 * pixel_idx + 0]      & 0xF0;
                u32 px_g = p_image->buffer[2 * pixel_idx + 0] << 4 & 0xF0;
                u32 px_b = p_image->buffer[2 * pixel_idx + 1]      & 0xF0;
                u32 px_a = p_image->buffer[2 * pixel_idx + 1] << 4 & 0xF0;
                res.buffer[pixel_idx] = ((77 * px_r + 150 * px_g + 29 * px_b) >> 8 & 0xF0) | px_a >> 4;
            }
        }
    }
    return res;
}

Image image_reduce(Image* p_image) {
    if (image_is_reduced(p_image)) {
        return image_copy(p_image);
    }
    if (p_image->kind == IMGK_RGBA) {
        Image res = image_create(p_image->width, p_image->height, IMGK_RGBA_REDUCED);
        for (u32 y = 0; y < p_image->height; y++) {
            for (u32 x = 0; x < p_image->width; x++) {
                u32 pixel_idx = y * p_image->width + x;
                res.buffer[2 * pixel_idx + 0] = (p_image->buffer[4 * pixel_idx + 0] & 0xF0) | (p_image->buffer[4 * pixel_idx + 1] >> 4);
                res.buffer[2 * pixel_idx + 1] = (p_image->buffer[4 * pixel_idx + 2] & 0xF0) | (p_image->buffer[4 * pixel_idx + 3] >> 4);
            }
        }
        return res;
    }
    else if (p_image->kind == IMGK_GRAY) {
        Image res = image_create(p_image->width, p_image->height, IMGK_GRAY_REDUCED);
        for (u32 y = 0; y < p_image->height; y++) {
            for (u32 x = 0; x < p_image->width; x++) {
                u32 pixel_idx = y * p_image->width + x;
                res.buffer[pixel_idx] = (p_image->buffer[2 * pixel_idx + 0] & 0xF0) | (p_image->buffer[2 * pixel_idx + 1] >> 4);
            }
        }
        return res;
    }
    else {
        panic("Unknown enum variant!");
    }
}

Image image_extend(Image* p_image) {
    if (!image_is_reduced(p_image)) {
        return image_copy(p_image);
    }
    if (p_image->kind == IMGK_RGBA_REDUCED) {
        Image res = image_create(p_image->width, p_image->height, IMGK_RGBA);
        for (u32 y = 0; y < p_image->height; y++) {
            for (u32 x = 0; x < p_image->width; x++) {
                u32 pixel_idx = y * p_image->width + x;
                res.buffer[4 * pixel_idx + 0] = p_image->buffer[2 * pixel_idx + 0] & 0xF0;
                res.buffer[4 * pixel_idx + 1] = p_image->buffer[2 * pixel_idx + 0] << 4;
                res.buffer[4 * pixel_idx + 2] = p_image->buffer[2 * pixel_idx + 1] & 0xF0;
                res.buffer[4 * pixel_idx + 3] = p_image->buffer[2 * pixel_idx + 1] << 4;
            }
        }
        return res;
    }
    else if (p_image->kind == IMGK_GRAY_REDUCED) {
        Image res = image_create(p_image->width, p_image->height, IMGK_GRAY);
        for (u32 y = 0; y < p_image->height; y++) {
            for (u32 x = 0; x < p_image->width; x++) {
                u32 pixel_idx = y * p_image->width + x;
                res.buffer[2 * pixel_idx + 0] = p_image->buffer[pixel_idx] & 0xF0;
                res.buffer[2 * pixel_idx + 1] = p_image->buffer[pixel_idx] << 4;
            }
        }
        return res;
    }
    else {
        panic("Unknown enum variant!");
    }
}

bool image_is_reduced(Image* p_image) {
    return p_image->kind == IMGK_RGBA_REDUCED || p_image->kind == IMGK_GRAY_REDUCED;
}

bool image_is_grayscale(Image* p_image) {
    return p_image->kind == IMGK_GRAY || p_image->kind == IMGK_GRAY_REDUCED;
}