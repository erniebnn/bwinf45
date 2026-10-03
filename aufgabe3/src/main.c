#include "gameconfig.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "panic.h"

bool game_determined(GameConfig* p_cfg, bool* p_game, u32 p_rnd) {
    if (p_rnd == 0) { return (p_cfg->target == 0) || (p_cfg->remaining_points[0] + p_cfg->points[0] == 0) ; }
    u32 cur_points = 0;
    for (u32 i = 0; i < p_rnd; i++) {
        cur_points += p_game[i] ? p_cfg->points[i] : 0;
    }
    if (cur_points >= p_cfg->target) { return true; }
    if (cur_points + p_cfg->remaining_points[p_rnd - 1] < p_cfg->target) { return true; }
    return false;
}

u32 game_otc(GameConfig* p_cfg, bool* p_game, bool* r_otc) {
    u64 cur_points = 0;
    for (u32 i = 0; i < p_cfg->rounds; i++) {
        if (p_game[i]) { cur_points += p_cfg->points[i]; }
        if (cur_points >= p_cfg->target) { *r_otc = true; return i; }
        if (cur_points + p_cfg->remaining_points[i] < p_cfg->target) { *r_otc = false; return i; }
    }
    ipanic("Unreachable code executed!");
}

void zero_game(GameConfig* p_cfg, bool* p_game) {
    for (u32 i = 0; i < p_cfg->rounds; i++) {
        p_game[i] = false;
    }
}

void increment_game(GameConfig* p_cfg, bool* p_game, u32 p_base) {
    bool carry = false;
    for (u32 i = p_base; i < p_cfg->rounds; i++) {
        bool a = p_game[i];
        bool b = (i == p_base);
        bool tmp = a != b;
        p_game[i] = tmp != carry;
        carry = (a && b) || (tmp && carry);
    }
}

void print_irrelevant(GameConfig* p_cfg, bool* p_game, u32 p_rnd) {
    for (u32 i = 0; i < p_cfg->rounds; i++) {
        if (i == p_rnd) {
            printf("X ");
            break;
        }
        else {
            printf(p_game[i] ? "W " : "L ");
        }
    }
}

void evaluate(GameConfig* p_cfg) {
    bool* cur_game = (bool*) calloc(p_cfg->rounds, sizeof(*cur_game));
    assert(cur_game != NULL, "Failed to allocate memory!");
    for (u32 rnd = 0; rnd < p_cfg->rounds; rnd++) {
        u32 postgame_len = p_cfg->rounds - rnd - 1;
        bool globally_irrelevant = true;
        for (u64 pregame = 0; pregame < (1 << rnd); pregame++) {
            if (!game_determined(p_cfg, cur_game, rnd)) {
                bool locally_irrelevant = true;
                for (u64 postgame = 0; postgame < (1 << postgame_len); postgame++) {
                    bool otc0, otc1;
                    cur_game[rnd] = false;
                    u32 len0 = game_otc(p_cfg, cur_game, &otc0);
                    cur_game[rnd] = true;
                    u32 len1 = game_otc(p_cfg, cur_game, &otc1);
                    // printf("len0 %d, len1 %d, otc0 %s, otc1 %s, Game: ", len0, len1, otc0 ? "W" : "L", otc1 ? "W" : "L"); print_locally_irrelevant(p_cfg, cur_game, (u32) -1); printf("\n");
                    if (otc0 != otc1 || len0 != len1) { locally_irrelevant = false; }
                    increment_game(p_cfg, cur_game, rnd + 1);
                }
                if (locally_irrelevant) {
                    printf("Locally irrelevant round %d: ", rnd);
                    print_irrelevant(p_cfg, cur_game, rnd);
                    printf("\n");
                }
                else {
                    globally_irrelevant = false;
                }
            }
            increment_game(p_cfg, cur_game, 0);
        }
        if (globally_irrelevant) {
            printf("Globally irrelevant round %d: ", rnd);
            print_irrelevant(p_cfg, cur_game, rnd);
            printf("\n");
        }
    }
    free(cur_game);
}

int main(int p_argc, const char** p_argv) {
    if (p_argc != 2) {
        panic("Usage: aufgabe3 <file>\nEvaluates the game configuration stored in <file>.");
    }
    GameConfig cfg = gameconfig_load(p_argv[1]);
    printf("Rounds: %d, Points: ", cfg.rounds);
    for (u32 i = 0; i < cfg.rounds; i++) {
        printf("%d ", cfg.points[i]);
    }
    printf(", Target: %d\n", cfg.target);
    evaluate(&cfg);
    gameconfig_free(&cfg);
    return 0;
}