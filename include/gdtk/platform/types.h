//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_TYPES_H
#define GDTK_TYPES_H

#include <stdint.h>

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

// Vectors

typedef union
{
  struct
  {
    S32 x, y;
  };
  S32 xy[2];
  struct
  {
    S32 w, h;
  };
  S32 wh[2];
} SVec2;

typedef union
{
  struct
  {
    U32 x, y;
  };
  U32 xy[2];
  struct
  {
    U32 w, h;
  };
  U32 wh[2];
} UVec2;


#endif // GDTK_TYPES_H
