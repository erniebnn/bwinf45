#include "gameconfig.h"
#include <stdlib.h>
#include <stdio.h>
#include "panic.h"

typedef enum _TokenKind {
    TK_NUMBER,
    TK_NEWLINE,
} TokenKind;

typedef struct _Token {
    TokenKind kind;
    u32 offset;
    u32 len;
    u32 value;
} Token;

static Token* tokenize(char* p_buf, u32* r_len) {
    u32 tokens_len = 0;
    u32 tokens_cap = 0;
    Token* tokens = NULL;
    char* cur = p_buf;
    while (*cur) {
        if (tokens_len == tokens_cap) {
            tokens_cap = tokens_cap ? 2 * tokens_cap : 4;
            tokens = (Token*) realloc((void*) tokens, tokens_cap * sizeof(*tokens));
            iassert(tokens != NULL, "Failed to allocate memory!");
        }
        if (*cur == '\n') {
            tokens[tokens_len++] = (Token) {
                .kind = TK_NEWLINE,
                .offset = (u32) (cur - p_buf),
                .len = 1,
            };
            cur++;
        } else if (*cur == ' ' || *cur == '\r' || *cur == '\t') {
            cur++;
        }
        else {
            char* next;
            u32 val = (u32) strtol(cur, &next, 0);
            if (cur < next) {
                tokens[tokens_len++] = (Token) {
                    .kind = TK_NUMBER,
                    .offset = (u32) (cur - p_buf),
                    .len = (u32) (next - p_buf),
                    .value = val,
                };
                cur = next;
            }
            else {
                panic("aufgabe3: Error while parsing game config file, unexpected character: '%c'", *cur);
            }
        }
    }
    tokens = (Token*) realloc((void*) tokens, tokens_len * sizeof(*tokens));
    iassert(tokens != NULL, "Failed to allocate memory!");
    *r_len = tokens_len;
    return tokens;
}

GameConfig gameconfig_load(const char* p_path) {
    char buf[4096];
    FILE* file = fopen(p_path, "r");
    assert(file != NULL, "aufgabe3: Failed to open '%s'!", p_path);
    u32 file_len = (u32) fread(buf, sizeof(char), 4096, file);
    if (file_len >= 4096) { panic("aufgabe3: Failed to read game config file, maximum of 4095 characters supported!"); }
    if (file_len == 0 && ferror(file)) { ipanic("Failed to read game config file due to an internal error!"); }
    buf[file_len] = '\0';
    fclose(file);
    u32 tokens_len;
    Token* tokens = tokenize(buf, &tokens_len);
    assert(tokens != NULL, "aufgabe3: '%s' is empty, it does not contain a valid game configuration!", p_path);
    Token* cur = tokens;
    while (cur < (tokens + tokens_len) && cur->kind == TK_NEWLINE) { cur++; }
    if (cur == (tokens + tokens_len)) { panic("aufgabe3: '%s' does not contain a number indicating the amount of rounds to play!", p_path); }
    u32 rounds = cur->value; cur++;
    assert(cur < (tokens + tokens_len), "aufgabe3: '%s' unexpected end of file, missing points per round data!", p_path);
    assert(cur->kind == TK_NEWLINE, "aufgabe3: '%s' expected a newline after the number of rounds!", p_path);
    while (cur < (tokens + tokens_len) && cur->kind == TK_NEWLINE) { cur++; }
    u32* points = (u32*) malloc(rounds * sizeof(*points));
    iassert(points != NULL, "Failed to allocate memory!");
    for (u32 i = 0; i < rounds; i++) {
        assert((cur + i) < (tokens + tokens_len), "aufgabe3: '%s' unexpected end of file, expected points per round data!", p_path);
        assert(cur[i].kind == TK_NUMBER, "aufgabe3: '%s' unexpected newline, expected points per round data!", p_path);
        points[i] = cur[i].value;
    }
    cur += rounds;
    assert(cur < (tokens + tokens_len), "aufgabe3: '%s' unexpected end of file, missing target points!", p_path);
    assert(cur->kind == TK_NEWLINE, "aufgabe3: '%s' expected a newline after the points per round!", p_path);
    while (cur < (tokens + tokens_len) && cur->kind == TK_NEWLINE) { cur++; }
    assert(cur < (tokens + tokens_len), "aufgabe3: '%s' unexpected end of file, missing target points data!", p_path);
    u32 target = cur->value;
    free(tokens);
    u32* remaining_points = (u32*) malloc(rounds * sizeof(*remaining_points));
    assert(remaining_points != NULL, "Failed to allocate memory!");
    u32 max_points = 0;
    for (u32 i = 0; i < rounds; i++) { max_points += points[i]; }
    for (u32 i = 0; i < rounds; i++) { remaining_points[i] = max_points; max_points -= points[i]; }
    return (GameConfig) {
        .rounds = rounds,
        .points = points,
        .remaining_points = remaining_points,
        .target = target,
    };
}

void gameconfig_free(GameConfig* p_gamecfg) {
    free(p_gamecfg->points);
}