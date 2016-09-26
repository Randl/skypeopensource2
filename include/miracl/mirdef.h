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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WIN64)
#define SOS_WIN
#else
#define SOS_NOTWIN
#endif


#ifdef SOS_WIN
#ifdef _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#ifdef __GNUC__
#if defined(__x86_64__) || defined(__ppc64__)
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#ifdef ENVIRONMENT32
#define MIRACL 32
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

#ifdef SOS_WIN
#define mr_dltype __int64   /* ... or long long for Unix/Linux */
#else
#define mr_dltype long long  
#endif
#define mr_unsign64 unsigned __int64

#define MAXBASE ((mr_small)1<<(MIRACL-1))
#else
#define MR_LITTLE_ENDIAN
#define MIRACL 64
#define mr_utype long
#define mr_unsign64 unsigned long
#define MR_IBITS 32
#define MR_LBITS 64
#define mr_unsign32 unsigned int
#define MR_FLASH 52
#define MAXBASE ((mr_small)1<<(MIRACL-1))
#define MR_BITSINCHAR 8

#ifdef SOS_WIN
#define mr_dltype __int64   /* ... or long long for Unix/Linux */
#else
#define mr_dltype long long  
#endif

#endif




