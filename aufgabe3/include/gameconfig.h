#pragma once
#include "typedefs.h"

typedef struct _GameConfig {
    u32 rounds;
    u32* points;
    u32 target;
} GameConfig;

GameConfig gameconfig_load(const char* p_path);

void gameconfig_free(GameConfig* p_gamecfg);