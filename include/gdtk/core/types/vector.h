//
// Created by Dwayne Edwards on 2026-06-21.
//

#ifndef GDTK_VECTOR_H
#define GDTK_VECTOR_H

#include "gdtk/core/defines.h"

typedef union
{
  struct
  {
    s32 x, y;
  };
  s32 xy[2];
  struct
  {
    s32 w, h;
  };
  s32 wh[2];
} sVec2;

typedef union
{
  struct
  {
    u32 x, y;
  };
  u32 xy[2];
  struct
  {
    u32 w, h;
  };
  u32 wh[2];
} uVec2;

#endif // GDTK_VECTOR_H
