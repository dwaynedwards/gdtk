//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_DEFINES_H
#define GDTK_DEFINES_H

#include <stdbool.h>
#include <stdint.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef bool b8;

#if defined(__APPLE__)
#define GDTK_PLATFORM_APPLE 1
#endif


#endif // GDTK_DEFINES_H
