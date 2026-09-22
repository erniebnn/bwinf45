#include "edgemap.h"
#include "rgba_histogram.h"
#include "typedefs.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "image.h"
#include "panic.h"

typedef struct _CompareCtx {
    bool help;
    bool dump_histogram;
    bool dump_edgemap;
    const char* file0;
    const char* file1;
} CompareCtx;

CompareCtx comparectx_from(u32 p_argc, const char** p_argv) {
    CompareCtx res = {};
    u32 filecnt = 0;
    for (u32 i = 0; i < p_argc; i++) {
        if (!strncmp(p_argv[i], "--", 2)) {
            if (!strcmp(p_argv[i], "--help")) {
                res.help = true;
            }
            else if (!strcmp(p_argv[i], "--dump-histogram")) {
                res.dump_histogram = true;
            }
            else if (!strcmp(p_argv[i], "--dump-edgemap")) {
                res.dump_edgemap = true;
            }
            else {
                panic("aufgabe2 compare: unrecognized option '%s'\nTry 'aufgabe2 compare --help' for more information.", p_argv[i]);
            }
        }
        else {
            if (filecnt == 0) {
                res.file0 = p_argv[i];
            }
            else if (filecnt == 1) {
                res.file1 = p_argv[i];
            }
            else {
                panic("aufgabe2 compare: too many files given, expected two.\nTry 'aufgabe2 compare --help' for more information.");
            }
            filecnt++;
        }
    }
    if (filecnt != 2 && !res.help) {
        panic("aufgabe2 comparae: not enough files given, expected two.\nTry 'aufgabe2 compare --help' for more information.");
    }
    return res;
}

void subcommand_compare(CompareCtx* p_ctx) {
    if (p_ctx->help) {
        printf("Usage: aufgabe2 compare [OPTION]... <file0> <file1>\nCompares both flags, giving a value between 0%% and 100%% alike.\nAvailable Options:\n  --help: show this message\n  --dump-histogram: dump the color histograms of both flags\n  --dump-edgemap: dump the edgemaps of both flags\n");
        exit(0);
    }
    Image flag0 = image_load(p_ctx->file0);
    Image flag1 = image_load(p_ctx->file1);
    RGBAHistogram hist0 = histogram_from(&flag0, 4);
    RGBAHistogram hist1 = histogram_from(&flag1, 4);
    EdgeMap edgm0 = edgemap_from(&flag0);
    EdgeMap edgm1 = edgemap_from(&flag1);
    edgemap_threshold(&edgm1, 32);
    edgemap_threshold(&edgm0, 32);
    edgemap_resize(&edgm0, 144, 48);
    edgemap_resize(&edgm1, 144, 48);
    edgemap_threshold(&edgm1, 1);
    edgemap_threshold(&edgm0, 1);
    if (p_ctx->dump_histogram) {
        histogram_dump(&hist0, stdout);
        histogram_dump(&hist1, stdout);
        printf("Histogram intersection: %.02f%%\n", 100.0 * histogram_compare(&hist0, &hist1));
    }
    if (p_ctx->dump_edgemap) {
        edgemap_dump(&edgm0, "edgemap0.png");
        edgemap_dump(&edgm1, "edgemap1.png");
        printf("Edgemap intersection: %.02f%%\n", 100.0 * edgemap_compare(&edgm0, &edgm1));
    }
    histogram_free(&hist0);
    histogram_free(&hist1);
    edgemap_free(&edgm0);
    edgemap_free(&edgm1);
}

int main(int p_argc, const char** p_argv) {
    if (p_argc < 2) {
        fprintf(stderr, "aufgabe2 needs a sub argument! either 'compare', 'findsimilar' or 'findpairs'\n");
        exit(-1);
    }
    if (!strcmp(p_argv[1], "compare")) {
        CompareCtx ctx = comparectx_from(p_argc - 2, p_argv + 2);
        subcommand_compare(&ctx);
    }
    else {
        fprintf(stderr, "Unknown suboption: %s, expected either 'compare', 'findsimilar' pr 'findpairs'\n", p_argv[1]);
        exit(-1);
    }
    return 0;
}