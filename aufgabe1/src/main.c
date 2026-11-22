#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "typedefs.h"
#include "panic.h"

// word3 -------v
// word2 -----v
//            X X
// word0 -> X X X X
// word1 -> X X X X
//            X X

typedef struct _Cross {
    u32 words[4][4];
    u32 wmask[12];
} Cross;

static u32** words = NULL;
static u32 words_len = 0;
static u32 words_cap = 0;

static bool check_single(u32 p_c, u32 p_mask) {
    return (p_c == p_mask) || !(p_c && p_mask);
}

static bool check_double(u32 p_c0, u32 p_c1, u32 p_mask) {
    if (p_mask) {
        return (p_c0 == p_mask) || (p_c1 == p_mask) || !(p_c0 && p_c1);
    }
    else {
        return (p_c0 == p_c1) || !(p_c0 && p_c1);
    }
}

static bool cross_check(Cross* p_cross) {
    bool matches = true;
    matches = matches && check_double(p_cross->words[0][1], p_cross->words[2][1], p_cross->wmask[3]);
    matches = matches && check_double(p_cross->words[0][2], p_cross->words[3][1], p_cross->wmask[4]);
    matches = matches && check_double(p_cross->words[1][1], p_cross->words[2][2], p_cross->wmask[7]);
    matches = matches && check_double(p_cross->words[1][2], p_cross->words[3][2], p_cross->wmask[8]);
    matches = matches && check_single(p_cross->words[2][0], p_cross->wmask[ 0]);
    matches = matches && check_single(p_cross->words[3][0], p_cross->wmask[ 1]);
    matches = matches && check_single(p_cross->words[0][0], p_cross->wmask[ 2]);
    matches = matches && check_single(p_cross->words[0][3], p_cross->wmask[ 5]);
    matches = matches && check_single(p_cross->words[1][0], p_cross->wmask[ 6]);
    matches = matches && check_single(p_cross->words[1][3], p_cross->wmask[ 9]);
    matches = matches && check_single(p_cross->words[2][3], p_cross->wmask[10]);
    matches = matches && check_single(p_cross->words[3][3], p_cross->wmask[11]);
    return matches;
}

static void set_word(Cross* p_cross, u32 p_word_idx, u32 p_word_id) {
    p_cross->words[p_word_idx][0] = words[p_word_id][0];
    p_cross->words[p_word_idx][1] = words[p_word_id][1];
    p_cross->words[p_word_idx][2] = words[p_word_id][2];
    p_cross->words[p_word_idx][3] = words[p_word_id][3];
}

static void clear_word(Cross* p_cross, u32 p_word_idx) {
    p_cross->words[p_word_idx][0] = 0;
    p_cross->words[p_word_idx][1] = 0;
    p_cross->words[p_word_idx][2] = 0;
    p_cross->words[p_word_idx][3] = 0;
}

static bool solve_cross_impl(Cross* p_cross, u32 p_idx) {
    if (p_idx == 4) {
        return true;
    }
    for (u32 i = 0; i < words_len; i++) {
        set_word(p_cross, p_idx, i);
        bool res = cross_check(p_cross);
        if (res) {
            res = solve_cross_impl(p_cross, p_idx + 1);
            if (res) { return true; }
        }
    }
    clear_word(p_cross, p_idx);
    return false;
}

static bool solve_cross(Cross* p_cross) {
    return solve_cross_impl(p_cross, 0);
}

static u32 decode_utf8(const char* p_src, u32 p_len, u32* r_consumed) {
    iassert(p_len > 0, "Can't decode UTF8 with length 0!");
    const unsigned char* src = (const unsigned char*) p_src;
    u32 res = 0;
    if ((src[0] & 0xF8) == 0xF0) {
        iassert(p_len >= 4, "Broken UTF8 sequence!");
        iassert((src[1] & 0xC0) == 0x80, "Broken UTF8 sequence!");
        iassert((src[2] & 0xC0) == 0x80, "Broken UTF8 sequence!");
        iassert((src[3] & 0xC0) == 0x80, "Broken UTF8 sequence!");
        res = (((u32) src[0] & 0x07) << 18) | (((u32) src[1] & 0x3F) << 12) | (((u32) src[2] & 0x3F) << 6) | ((u32) src[3] &0x3F);
        *r_consumed = 4;
    }
    else if ((src[0] & 0xF0) == 0xE0) {
        iassert(p_len >= 3, "Broken UTF8 sequence!");
        iassert((src[1] & 0xC0) == 0x80, "Broken UTF8 sequence!");
        iassert((src[2] & 0xC0) == 0x80, "Broken UTF8 sequence!");
        res = (((u32) src[0] & 0x0F) << 12) | (((u32) src[1] & 0x3F) << 6) | ((u32) src[2] & 0x3F);
        *r_consumed = 3;
    }
    else if ((src[0] & 0xE0) == 0xC0) {
        iassert(p_len >= 2, "Broken UTF8 sequence!");
        iassert((src[1] & 0xC0) == 0x80, "Broken UTF8 sequence!");
        res = (((u32) src[0] & 0x1F) << 6) | ((u32) src[1] & 0x3F);
        *r_consumed = 2;
    }
    else if ((src[0] & 0x80) == 0x00) {
        res = (u32) src[0];
        *r_consumed = 1;
    }
    else {
        ipanic("Broken UTF8 sequence!");
    }
    return res;
}

static u32 decode_word(const char* p_src, u32 p_len, u32* r_dest) {
    u32 len = (u32) strlen(p_src);
    if (len < p_len) { return 0; }
    const char* cur = p_src;
    for (u32 i = 0; i < p_len; i++) {
        u32 consumed;
        r_dest[i] = decode_utf8(cur, len, &consumed);
        cur += consumed; len -= consumed;
        if (len < (p_len - i - 1)) { return 0; }
    }
    return (u32) (cur - p_src);
}

static void print_utf8(u32 p_char, FILE* p_file) {
    char buf[5];
    if (p_char <= 0x7F) { // single byte
        buf[0] = (char) p_char;
        buf[1] = '\0';
    }
    else if (p_char <= 0x7FF) { // two bytes
        buf[0] = (char) 0xC0 | (char) (p_char >>  6 & 0x1F);
        buf[1] = (char) 0x80 | (char) (p_char       & 0x3F);
        buf[2] = '\0';
    }
    else if (p_char <= 0xFFFF) { // three bytes
        buf[0] = (char) 0xE0 | (char) (p_char >> 12 & 0x0F);
        buf[1] = (char) 0x80 | (char) (p_char >>  6 & 0x3F);
        buf[2] = (char) 0x80 | (char) (p_char       & 0x3F);
        buf[3] = '\0';
    }
    else if (p_char <= 0x1FFFFF) { // four bytes
        buf[0] = (char) 0xF0 | (char) (p_char >> 18 & 0x07);
        buf[1] = (char) 0x80 | (char) (p_char >> 12 & 0x3F);
        buf[2] = (char) 0x80 | (char) (p_char >>  6 & 0x3F);
        buf[3] = (char) 0x80 | (char) (p_char       & 0x3F);
        buf[4] = '\0';
    }
    fprintf(p_file, "%s", buf);
}

static void print_cross(Cross* p_cross, FILE* p_file) {
    fprintf(p_file, "Word 0: ");
    print_utf8(p_cross->words[0][0], p_file);
    print_utf8(p_cross->words[0][1], p_file);
    print_utf8(p_cross->words[0][2], p_file);
    print_utf8(p_cross->words[0][3], p_file);
    fprintf(p_file, "\nWord 1: ");
    print_utf8(p_cross->words[1][0], p_file);
    print_utf8(p_cross->words[1][1], p_file);
    print_utf8(p_cross->words[1][2], p_file);
    print_utf8(p_cross->words[1][3], p_file);
    fprintf(p_file, "\nWord 2: ");
    print_utf8(p_cross->words[2][0], p_file);
    print_utf8(p_cross->words[2][1], p_file);
    print_utf8(p_cross->words[2][2], p_file);
    print_utf8(p_cross->words[2][3], p_file);
    fprintf(p_file, "\nWord 3: ");
    print_utf8(p_cross->words[3][0], p_file);
    print_utf8(p_cross->words[3][1], p_file);
    print_utf8(p_cross->words[3][2], p_file);
    print_utf8(p_cross->words[3][3], p_file);
    fprintf(p_file, "\n");
}

static void load_words(const char* p_path) {
    FILE* file = fopen(p_path, "rb");
    assert(file != NULL, "Failed to open file: %s!", p_path);
    int result;
    char buf[64];
    while ((result = fscanf(file, "%63s", buf)) == 1) {
        if (words_len == words_cap) {
            words_cap = words_cap ? 2 * words_cap : 64;
            words = (u32**) realloc((void*) words, words_cap * sizeof(*words));
            iassert(words != NULL, "Failed to allocate memory!");
        }
        u32* word = (u32*) malloc(4 * sizeof(*word));
        iassert(word != NULL, "Failed to allocate memory!");
        u32 len = (u32) strlen(buf);
        u32 res = decode_word(buf, 4, word);
        assert(res == len, "Failed to read word '%s' from %s in line %d. All words must be 4 UTF8 codepoints long!", buf, p_path, words_len + 1);
        words[words_len++] = word;
    }
    if (result != EOF) {
        panic("Failed to fully parse file '%s'!", p_path);
    }
    fclose(file);
}

static void free_words() {
    for (u32 i = 0; i < words_len; i++) {
        free(words[i]);
    }
    free(words);
}

static void load_cross(const char* p_path, Cross* r_cross) {
    FILE* file = fopen(p_path, "rb");
    assert(file != NULL, "Failed to open file: %s!", p_path);
    char buf[64];
    u32 mask_idx = 0;
    Cross cross = {};
    for (u32 i = 0; i < 4; i++) {
        int res = fscanf(file, "%63s", buf);
        assert(res == 1, "Failed to parse cross in '%s'!", p_path);
        u32 consumed;
        u32 len = (u32) strlen(buf);
        char* cur = buf;
        for (u32 j = 0; j < 4; j++) {
            u32 c = decode_utf8(cur, len, &consumed); cur += consumed; len -= consumed;
            if ((i == 0 || i == 3) && (j == 0 || j == 3)) {
                assert(c == (u32) (unsigned char) '#', "Failed to parse cross in '%s'!", p_path);
            }
            else {
                cross.wmask[mask_idx++] = (c == (u32) (unsigned char) '.' ? 0 : c);
            }
        }
        assert(len == 0, "Failed to parse cross in '%s'!", p_path);
    }
    int ch;
    do { ch = fgetc(file); } while (ch == '\r' || ch == '\n');
    assert(fgetc(file) == EOF, "Failed to parse cross in '%s'. Unexpected data after the end of the cross!", p_path);
    fclose(file);
    *r_cross = cross;
}

int main(int p_argc, const char** p_argv) {
    assert(p_argc == 3, "Usage: %s <wordlist> <cross>\n<wordlist> is a file with the list of words to use, <cross> is a file containing the cross to be solved.", p_argv[0]);
    load_words(p_argv[1]);
    Cross cross;
    load_cross(p_argv[2], &cross);
    bool res = solve_cross(&cross);
    if (res) {
        printf("Solved cross:\n");
        print_cross(&cross, stdout);
    }
    else {
        printf("The cross is unsolveable with the given wordlist!\n");
    }
    free_words();
}