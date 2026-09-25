#include "edgemap.h"
#include "pixelcomp.h"
#include "rgba_histogram.h"
#include "typedefs.h"
#include <stddef.h>
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

typedef struct _FindSimilarCtx {
    bool help;
    const char* file0;
    const char* dir0;
} FindSimilarCtx;

typedef struct _FindPairsCtx {
    bool help;
    bool detailed;
    const char* dir0;
} FindPairsCtx;

typedef struct _ComparisionInstance {
    Image image;
    RGBAHistogram histogram;
    EdgeMap edgemap;
} ComparisionInstance;

typedef struct _ComparedEntry {
    const char* file0;
    const char* file1;
    double total_score;
    double hist_score;
    double edge_score;
    double pixel_score;
} ComparedEntry;

static CompareCtx comparectx_from(u32 p_argc, const char** p_argv) {
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
        panic("aufgabe2 compare: not enough files given, expected two.\nTry 'aufgabe2 compare --help' for more information.");
    }
    return res;
}

static FindSimilarCtx findsimilarctx_from(u32 p_argc, const char** p_argv) {
    FindSimilarCtx res = {};
    for (u32 i = 0; i < p_argc; i++) {
        if (!strncmp(p_argv[i], "--", 2)) {
            if (!strcmp(p_argv[i], "--help")) {
                res.help = true;
            }
            else {
                panic("aufgabe2 findsimilar: unrecognized option '%s'\nTry 'aufgabe2 findsimilar --help' for more information.", p_argv[i]);
            }
        }
        else {
            if (res.file0 == NULL) {
                res.file0 = p_argv[i];
            }
            else if (res.dir0 == NULL) {
                res.dir0 = p_argv[i];
            }
            else {
                panic("aufgabe2 findsimilar: too many filepaths, expected two.\nTry 'aufgabe2 findsimilar --help' for more information.");
            }
        }
    }
    if (res.dir0 == NULL && !res.help) {
        panic("aufgabe2 findsimilar: not enough filepaths given, expected two.\nTry 'aufgabe2 findsimilar --help' for more information.");
    }
    return res;
}

static FindPairsCtx findpairsctx_from(u32 p_argc, const char** p_argv) {
    FindPairsCtx res = {};
    for (u32 i = 0; i < p_argc; i++) {
        if (!strncmp(p_argv[i], "--", 2)) {
            if (!strcmp(p_argv[i], "--help")) {
                res.help = true;
            }
            else if (!strcmp(p_argv[i], "--detailed")) {
                res.detailed = true;
            }
            else {
                panic("aufgabe2 findpairs: unrecognized option '%s'\nTry 'aufgabe2 findpairs --help' for more information.", p_argv[i]);
            }
        }
        else {
            if (res.dir0 == NULL) {
                res.dir0 = p_argv[i];
            }
            else {
                panic("aufgabe2 findpairs: too many paths, expected one.\nTry 'aufgabe2 findpairs --help' for more information.");
            }
        }
    }
    if (res.dir0 == NULL && !res.help) {
        panic("aufgabe2 findpairs: not enough paths given, expected one.\nTry 'aufgabe2 findpairs --help' for more information.");
    }
    return res;
}

static char** collect_files(const char* p_path, u32* r_len) {
    DIR* dir = opendir(p_path);
    if (!dir) { perror("opendir"); exit(-1); }
    u32 files_cap = 4;
    u32 files_len = 0;
    char** files = (char**) malloc(files_cap * sizeof(*files));
    iassert(files != NULL, "Failed to allocate memory!");
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (files_len == files_cap) {
            files_cap = 2 * files_cap;
            files = (char**) realloc((void*) files, files_cap * sizeof(*files));
        }
        if (entry->d_type == DT_REG) {
            files[files_len] = (char*) malloc(512 * sizeof(char));
            iassert(files[files_len] != NULL, "Failed to allocate memory!");
            int res = snprintf(files[files_len], 512, "%s/%s", p_path, entry->d_name);
            if (res > 512 || res < 0) { ipanic("Failed to collect filename (path too long?)"); }
            files_len++;
        }
    }
    closedir(dir);
    *r_len = files_len;
    files = (char**) realloc((void*) files, files_len * sizeof(*files));
    iassert(files != NULL, "Failed to allocate memory!");
    return files;
}

static ComparisionInstance compinstance_from(const char* p_file) {
    ComparisionInstance res = {};
    res.image = image_load(p_file);
    res.histogram = histogram_from(&res.image, 4);
    res.edgemap = edgemap_from(&res.image);
    edgemap_threshold(&res.edgemap, 32);
    edgemap_resize(&res.edgemap, 144, 48);
    edgemap_threshold(&res.edgemap, 1);
    return res;
}

static void compinstance_free(ComparisionInstance* p_inst) {
    histogram_free(&p_inst->histogram);
    edgemap_free(&p_inst->edgemap);
    image_free(&p_inst->image);
}

static double compare_flags(ComparisionInstance* p_inst0, ComparisionInstance* p_inst1, double* r_hist_score, double* r_edge_score, double* r_pixel_score) {
    double hist_score = histogram_compare(&p_inst0->histogram, &p_inst1->histogram);
    double edge_score = edgemap_compare(&p_inst0->edgemap, &p_inst1->edgemap);
    double pixel_score = image_pixel_compare(&p_inst0->image, &p_inst1->image);
    double hist_weight = 0.35;
    double edge_weight = 0.25;
    double pixel_weight = 0.40;
    double total_score = hist_weight * hist_score + edge_weight * edge_score + pixel_weight * pixel_score;
    if (r_hist_score) { *r_hist_score = hist_score; }
    if (r_edge_score) { *r_edge_score = edge_score; }
    if (r_pixel_score) { *r_pixel_score = pixel_score; }
    return total_score;
}

static void subcommand_compare(CompareCtx* p_ctx) {
    if (p_ctx->help) {
        printf("Usage: aufgabe2 compare [OPTION]... <file0> <file1>\nCompares both flags, giving a value between 0%% and 100%% alike.\nAvailable Options:\n  --help: show this message\n  --dump-histogram: dump the color histograms of both flags\n  --dump-edgemap: dump the edgemaps of both flags\n");
        exit(0);
    }
    ComparisionInstance inst0 = compinstance_from(p_ctx->file0);
    ComparisionInstance inst1 = compinstance_from(p_ctx->file1);
    double hist_score, edge_score, pixel_score;
    double total_score = compare_flags(&inst0, &inst1, &hist_score, &edge_score, &pixel_score);
    if (p_ctx->dump_histogram) {
        histogram_dump(&inst0.histogram, stdout);
        histogram_dump(&inst1.histogram, stdout);
    }
    if (p_ctx->dump_edgemap) {
        edgemap_dump(&inst0.edgemap, "edgemap0.png");
        edgemap_dump(&inst1.edgemap, "edgemap1.png");
    }
    printf("Histogram intersection: %.02f%%\n", 100.0 * hist_score);
    printf("Edgemap intersection: %.02f%%\n", 100.0 * edge_score);
    printf("Pixel Intersection: %.02f%%\n", 100.0 * pixel_score);
    printf("Total Intersection: %.02f%%\n", 100.0 * total_score);
    compinstance_free(&inst0);
    compinstance_free(&inst1);
}

// Very simple bubble sort
static void sort_comparedentry_list(ComparedEntry* p_entries, u32 p_len) {
    for (u32 i = 0; i < p_len - 1; i++) {
        for (u32 j = 0; j < p_len - i - 1; j++) {
            if (p_entries[j].total_score > p_entries[j + 1].total_score) {
                ComparedEntry tmp = p_entries[j];
                p_entries[j] = p_entries[j + 1];
                p_entries[j + 1] = tmp;
            }
        }
    }
}

static void subcommand_findsimilar(FindSimilarCtx* p_ctx) {
    if (p_ctx->help) {
        printf("Usage: aufgabe2 findsimilar [OPTION]... <file> <dir>\nCompares the given flag with all flags inside the directory, excluding the given flag if it is contained in the target directory.\nAvailable Options:\n  --help: show this message\n");
        exit(0);
    }
    ComparisionInstance inst0 = compinstance_from(p_ctx->file0);
    u32 files_len;
    char** files = collect_files(p_ctx->dir0, &files_len);
    assert(files_len > 0, "aufgabe2 findsimilar: the given folder needs to contain at least 1 file to compare to!");
    ComparisionInstance* compinstances = (ComparisionInstance*) malloc(files_len * sizeof(*compinstances));
    iassert(compinstances != NULL, "Failed to allocate memory!");
    for (u32 i = 0; i < files_len; i++) {
        compinstances[i] = compinstance_from(files[i]);
    }
    ComparedEntry* entries = (ComparedEntry*) malloc(files_len * sizeof(*entries));
    iassert(entries != NULL, "Failed to allocate memory!");
    bool contains_itself = false;
    for (u32 i = 0; i < files_len; i++) {
        double hist_score, edge_score, pixel_score;
        entries[i] = (ComparedEntry) {
            .file0 = p_ctx->file0,
            .file1 = files[i],
            .total_score = compare_flags(&inst0, &compinstances[i], &hist_score, &edge_score, &pixel_score),
            .hist_score = hist_score,
            .edge_score = edge_score,
            .pixel_score = pixel_score,
        };
        if (!strcmp(p_ctx->file0, files[i])) { contains_itself = true; }
        compinstance_free(&compinstances[i]);
    }
    free(compinstances);
    compinstance_free(&inst0);
    sort_comparedentry_list(entries, files_len);
    u32 lower = contains_itself ? (files_len > 6 ? files_len - 6 : 0) : (files_len > 5 ? files_len - 5 : 0); // awful trickery for lower bound
    u32 upper = contains_itself ? files_len - 1 : files_len; // skip top entry when the given folder contains the compared against file, as the top entry will be it compared against itself
    for (u32 i = lower; i < upper; i++) {
        ComparedEntry entry = entries[i];
        printf("%s <- %.02f%% -> %s\n", entry.file0, 100.0 * entry.total_score, entry.file1);
    }
    for (u32 i = 0; i < files_len; i++) { free(files[i]); }
    free(entries);
}

static void subcommand_findpairs(FindPairsCtx* p_ctx) {
    if (p_ctx->help) {
        printf("Usage: aufgabe2 findpairs [OPTION]... <dir>\nCompares the all flags with all flags inside the directory, excluding the themselves.\nAvailable Options:\n  --help: show this message\n  --detailed: show detailed scoring information\n");
        exit(0);
    }
    u32 files_len;
    char** files = collect_files(p_ctx->dir0, &files_len);
    assert(files_len > 1, "aufgabe2 findsimilar: the given folder needs to contain at least 2 files to compare!");
    ComparisionInstance* compinstances = (ComparisionInstance*) malloc(files_len * sizeof(*compinstances));
    iassert(compinstances != NULL, "Failed to allocate memory!");
    for (u32 i = 0; i < files_len; i++) {
        compinstances[i] = compinstance_from(files[i]);
    }
    u32 entries_len = files_len * (files_len - 1) / 2; // All unordered pairs of images, excluding themselves
    ComparedEntry* entries = (ComparedEntry*) malloc(entries_len * sizeof(*entries));
    iassert(entries != NULL, "Failed to allocate memory!");
    u32 cnt = 0;
    for (u32 i = 0; i < files_len; i++) {
        for (u32 j = i + 1; j < files_len; j++) {
            double hist_score, edge_score, pixel_score;
            entries[cnt] = (ComparedEntry) {
                .file0 = files[i],
                .file1 = files[j],
                .total_score = compare_flags(&compinstances[i], &compinstances[j], &hist_score, &edge_score, &pixel_score),
                .hist_score = hist_score,
                .edge_score = edge_score,
                .pixel_score = pixel_score,
            };
            cnt++;
        }
        compinstance_free(&compinstances[i]);
        printf("\rCalculating... %.02f%%", 100.0 * (double) cnt / (double) entries_len);
        fflush(stdout);
    }
    printf("\n");
    sort_comparedentry_list(entries, entries_len);
    for (u32 i = (entries_len > 10) ? entries_len - 10 : 0 ; i < entries_len; i++) {
        ComparedEntry entry = entries[i];
        if (p_ctx->detailed) {
            printf(
                "Total: %.02f%%, Histogram: %.02f%%, Edgemap: %.02f%%, Per-Pixel: %.02f%%, '%s' and '%s'\n",
                100.0 * entry.total_score,
                100.0 * entry.hist_score,
                100.0 * entry.edge_score,
                100.0 * entry.pixel_score,
                entry.file0,
                entry.file1
            );
        }
        else {
            printf("%s <- %.02f%% -> %s\n", entry.file0, 100.0 * entry.total_score, entry.file1);
        }
    }
    free(entries);
    for (u32 i = 0; i < files_len; i++) {
        free(files[i]);
    }
    free(files);
}

int main(int p_argc, const char** p_argv) {
    if (p_argc < 2) {
        fprintf(stderr, "aufgabe2 needs a suboption! either 'compare', 'findsimilar' or 'findpairs'\n");
        exit(-1);
    }
    if (!strcmp(p_argv[1], "compare")) {
        CompareCtx ctx = comparectx_from(p_argc - 2, p_argv + 2);
        subcommand_compare(&ctx);
    }
    else if (!strcmp(p_argv[1], "findsimilar")) {
        FindSimilarCtx ctx = findsimilarctx_from(p_argc - 2, p_argv + 2);
        subcommand_findsimilar(&ctx);
    }
    else if (!strcmp(p_argv[1], "findpairs")) {
        FindPairsCtx ctx = findpairsctx_from(p_argc - 2, p_argv + 2);
        subcommand_findpairs(&ctx);
    }
    else {
        fprintf(stderr, "Unknown suboption: %s, expected either 'compare', 'findsimilar' or 'findpairs'\n", p_argv[1]);
        exit(-1);
    }
    return 0;
}