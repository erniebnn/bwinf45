#pragma once

#ifndef __PANIC_H__
#define __PANIC_H__

#include <stdio.h>
#undef assert

#define panic(fmt, ...) do { fprintf(stderr, (fmt "\n") __VA_OPT__(,) __VA_ARGS__); exit(-1); } while (0)
#define assert(cond, fmt, ...) do { if (!(cond)) { fprintf(stderr, (fmt "\n") __VA_OPT__(,) __VA_ARGS__); exit(-1); } } while(0)

#define ipanic(fmt, ...) do { fprintf(stderr, ("[%s:%d] " fmt "\n"), __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__); exit(-1); } while (0)
#define iassert(cond, fmt, ...) do { if (!(cond)) { fprintf(stderr, ("[%s:%d] " fmt "\n"), __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__); exit(-1); } } while(0)

#endif // #ifndef __PANIC_H__