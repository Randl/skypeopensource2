/* 
 *   MIRACL compiler/hardware definitions - mirdef.h
 *   This version suitable for use with most 32-bit computers
 *   e.g. 80386+ PC, VAX, ARM etc. Assembly language versions of muldiv,
 *   muldvm, muldvd and muldvd2 will be necessary. See mrmuldv.any 
 *
 *   Also suitable for DJGPP GNU C Compiler
 *   ... but change __int64 to long long
 *   Copyright (c) 1988-2006 Shamus Software Ltd.
 */
// TODO: move to common utils
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#ifdef ENVIRONMENT32
#define MIRACL 32
#else
#define MIRACL 64
#endif

#define MR_LITTLE_ENDIAN    /* This may need to be changed        */
#define mr_utype int
/* the underlying type is usually int *
 * but see mrmuldv.any                */
#define mr_unsign32 unsigned int
/* 32 bit unsigned type               */
#define MR_IBITS      32    /* bits in int  */
#define MR_LBITS      32    /* bits in long */
#define MR_FLASH      52
/* delete this definition if integer  *
 * only version of MIRACL required    */
/* Number of bits per double mantissa */

#define mr_dltype __int64   /* ... or long long for Unix/Linux */
#define mr_unsign64 unsigned __int64

#define MAXBASE ((mr_small)1<<(MIRACL-1))


