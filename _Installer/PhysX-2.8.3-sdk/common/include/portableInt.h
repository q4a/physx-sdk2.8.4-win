/*----------------------------------------------------------------------------*\
|
|                                NVIDIA Technology
|
|                                 www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef PORTABLE_INT_H
#define PORTABLE_INT_H 1


/*---------------------------------------------------------------------*/
/**
 *  Portable type definitions ONLY -- No methods or function prototypes!
 */

/*---------------------------------------------------------------------*/
/**
 *  Portable type definitions for integers...
 */


typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;

typedef signed char     s8;
typedef signed short    s16;
typedef signed long     s32;

typedef struct
{
	u8 *  elt;
	u32   numElts;
	u32   maxElts;
} 
u8vector;

typedef struct
{
	u16 *  elt;
	u32    numElts;
	u32    maxElts;
}
u16vector;

typedef struct
{
	u32 *  elt;
	u32    numElts;
	u32    maxElts;
}
u32vector;

typedef struct
{
	s8 *  elt;
	u32   numElts;
	u32   maxElts;
} 
s8vector;

typedef struct
{
	s16 *  elt;
	u32    numElts;
	u32    maxElts;
}
s16vector;

typedef struct
{
	s32 *  elt;
	u32    numElts;
	u32    maxElts;
}
s32vector;


/*---------------------------------------------------------------------*/
/**
 *  Portable type definitions for integer vectors...
 */


typedef struct
{
    s16    x;
    s16    y;
    s16    z;
}
s16v3;

typedef struct
{
	s16v3 *  elt;
	u32      numElts;
	u32      maxElts;
}
s16v3vector;

typedef struct
{
    s16    x;
    s16    y;
    s16    z;
    s16    w;
}
s16v4;

typedef struct
{
	s16v4 *  elt;
	u32      numElts;
	u32      maxElts;
}
s16v4vector;

typedef struct
{
    s32    x;
    s32    y;
    s32    z;
}
s32v3;

typedef struct
{
	s32v3 *  elt;
	u32      numElts;
	u32      maxElts;
}
s32v3vector;

typedef struct
{
    s32    x;
    s32    y;
    s32    z;
    s32    w;
}
s32v4;

typedef struct
{
    s32v4 *  elt;
	u32      numElts;
	u32      maxElts;
}
s32v4vector;



#endif /* PORTABLE_INT_H */
