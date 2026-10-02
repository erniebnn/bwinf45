#include "gameconfig.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

u32 game_otc(GameConfig* p_cfg, u64 p_game, bool* r_otc) {
    u64 cur_points = 0;
    for (u32 i = 0; i < 64; i++) {
        if (cur_points >= p_cfg->target) { *r_otc = true; return i; }
        if (cur_points + p_cfg->remaining_points[i] < p_cfg->target) { *r_otc = false; return i; }
        if (p_game & 0b1) { cur_points += p_cfg->points[i]; }
        p_game = p_game >> 1;
    }
    return 64;
}

void evaluate(GameConfig* p_cfg, u32* r_relevance) {
    for (u32 rnd = 0; rnd < p_cfg->rounds; rnd++) {
        u32 postgame_len = p_cfg->rounds - rnd - 1;
        for (u64 pregame = 0; pregame < (1 << rnd); pregame++) {
            bool locally_irrelevant = true;
            for (u64 postgame = 0; postgame < (1 << postgame_len); postgame++) {
                u64 wingame  = pregame | (1 << rnd) | postgame;
                u64 losegame = pregame |          0 | postgame;
                bool otc0, otc1;
                u32 len0 = game_otc(p_cfg, wingame , &otc0);
                u32 len1 = game_otc(p_cfg, losegame, &otc1);
                if (otc0 != otc1 || len0 != len1) { locally_irrelevant = false; }
            }
            if (!locally_irrelevant) { r_relevance[rnd] += 1; }
        }
    }
}

int main(int p_argc, const char** p_argv) {
    GameConfig cfg = gameconfig_load(p_argv[1]);
    printf("Rounds: %d, Points: ", cfg.rounds);
    for (u32 i = 0; i < cfg.rounds; i++) {
        printf("%d ", cfg.points[i]);
    }
    printf(", Target: %d\n", cfg.target);
    u32* relevant_list = (u32*) calloc(cfg.rounds, sizeof(*relevant_list));
    evaluate(&cfg, relevant_list);
    for (u32 i = 0; i < cfg.rounds; i++) {
        printf("Round %d: %d\n", i, relevant_list[i]);
    }
    gameconfig_free(&cfg);
    return 0;
}