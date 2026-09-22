#pragma once
#include "typedefs.h"
#include <stdbool.h>

typedef enum _ImageKind {
    IMGK_RGBA,
    IMGK_GRAY,
} ImageKind;

typedef struct _Image {
    ImageKind kind;
    u8* buffer;
    u32 width;
    u32 height;
} Image;

Image image_create(u32 p_width, u32 p_height, ImageKind p_kind);
Image image_load(const char* p_path);
void image_save(Image* p_image, const char* p_path);
void image_free(Image* p_image);
Image image_copy(Image* p_image);

Image image_grayscale(Image* p_image);

bool image_is_grayscale(Image* p_image);