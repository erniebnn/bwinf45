#include "gameconfig.h"
#include <stdlib.h>
#include <stdio.h>

GameConfig gameconfig_load(const char* p_path) {
    FILE* file = fopen(p_path, "r");
}

void gameconfig_free(GameConfig* p_gamecfg) {
    free(p_gamecfg->points);
}