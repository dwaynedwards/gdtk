//
// Created by Dwayne Edwards on 2026-06-20.
//

#ifndef GDTK_EVENTS_H
#define GDTK_EVENTS_H

typedef enum
{
  GDTK_EVENT_TYPE_UNUSED = 0
} GDTK_EventType;

typedef union
{
  GDTK_EventType type;
} GDTK_Event;

#endif // GDTK_EVENTS_H
