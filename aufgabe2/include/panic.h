#pragma once
#include <stdlib.h>
#include <stdio.h>
#undef assert

#define panic(fmt, ...) do { fprintf(stderr, (fmt "\n") __VA_OPT__(,) __VA_ARGS__); exit(-1); } while (0)
#define assert(cond, fmt, ...) do { if (!(cond)) { fprintf(stderr, (fmt "\n") __VA_OPT__(,) __VA_ARGS__); exit(-1); } } while(0)