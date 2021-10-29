
#ifndef TYPEDEF_H
#define TYPEDEF_H

#if  ARDUINO >= 100
//WProgram renamed to Arduino.h. in new IDE 1.5.2
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

typedef boolean            BOOL;
typedef unsigned int       UINT16;
typedef signed int         SINT16;
typedef byte               UINT8;
typedef signed long        SINT32;
typedef unsigned long      UINT32;
typedef signed long long   SINT64;
typedef unsigned long long UINT64;

#endif

