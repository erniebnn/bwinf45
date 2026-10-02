#pragma once
#include "typedefs.h"

typedef struct _Game {
    u32 rounds;
    u32 current;
    bool* outcomes;
} Game;

Game* games_construct(u32 rounds);