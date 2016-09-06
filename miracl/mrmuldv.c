/*
 *  Borland C++ 32-bit compiler (BCC32). Use with mirdef.h32 
 *  Uses inline assembly feature. Suitable for Win32 Apps
 *  Also compatible with Microsoft Visual C++ 32-bit compiler
 */
#include <stdio.h>
#include "miracl.h"

#ifdef INLINE_ASM
#if INLINE_ASM == 3
#define ASM _asm
int muldiv(a,b,c,m,rp)
int a,b,c,m,*rp;
{
        ASM mov   eax,DWORD PTR a      
        ASM mul   DWORD PTR b          
        ASM add   eax,DWORD PTR c      
        ASM adc   edx,0h                 
        ASM div   DWORD PTR m          
        ASM mov   ebx,DWORD PTR rp     
        ASM mov   [ebx],edx              
}

int muldvm(a,c,m,rp)
int a,c,m,*rp;
{
        ASM mov   edx,DWORD PTR a      
        ASM mov   eax,DWORD PTR c      
        ASM div   DWORD PTR m          
        ASM mov   ebx,DWORD PTR rp     
        ASM mov   [ebx],edx              
}

int muldvd(a,b,c,rp)
int a,b,c,*rp;
{
        ASM mov   eax,DWORD PTR a      
        ASM mul   DWORD PTR b          
        ASM add   eax,DWORD PTR c      
        ASM adc   edx,0h                 
        ASM mov   ebx,DWORD PTR rp     
        ASM mov   [ebx],eax              
        ASM mov   eax,edx
}

void muldvd2(a,b,c,rp)
int a,b,*c,*rp;
{
        ASM mov   eax,DWORD PTR a      
        ASM mul   DWORD PTR b          
        ASM mov   ebx,DWORD PTR c
        ASM add   eax,[ebx]
        ASM adc   edx,0h
        ASM mov   esi,DWORD PTR rp
        ASM add   eax,[esi]
        ASM adc   edx,0h
        ASM mov   [esi],eax              
        ASM mov   [ebx],edx
}
#else
mr_small muldiv(mr_small a,mr_small b,mr_small c,mr_small m,mr_small *rp)
{
    mr_small q;
    __asm__ __volatile__ (
    "movl %1,%%eax\n"
    "mull %2\n"
    "addl %3,%%eax\n"
    "adcl $0,%%edx\n"
    "divl %4\n"
    "movl %5,%%ebx\n"
    "movl %%edx,(%%ebx)\n"
    "movl %%eax,%0\n"
    : "=m"(q)
    : "m"(a),"m"(b),"m"(c),"m"(m),"m"(rp)
    : "eax","ebx","memory"
    );
    return q;
}

mr_small muldvm(mr_small a,mr_small c,mr_small m,mr_small *rp)
{
    mr_small q;
    __asm__ __volatile__ (
    "movl %1,%%edx\n"
    "movl %2,%%eax\n"
    "divl %3\n"
    "movl %4,%%ebx\n"
    "movl %%edx,(%%ebx)\n"
    "movl %%eax,%0\n"
    : "=m"(q)
    : "m"(a),"m"(c),"m"(m),"m"(rp)
    : "eax","ebx","memory"
    );
    return q;
}

mr_small muldvd(mr_small a,mr_small b,mr_small c,mr_small *rp)
{
    mr_small q;
    __asm__ __volatile__ (
    "movl %1,%%eax\n"
    "mull %2\n"
    "addl %3,%%eax\n"
    "adcl $0,%%edx\n"
    "movl %4,%%ebx\n"
    "movl %%eax,(%%ebx)\n"
    "movl %%edx,%0\n"
    : "=m"(q)
    : "m"(a),"m"(b),"m"(c),"m"(rp)
    : "eax","ebx","memory"
    );
    return q;
}

void muldvd2(mr_small a,mr_small b,mr_small *c,mr_small *rp)
{
    __asm__ __volatile__ (
    "movl %0,%%eax\n"
    "mull %1\n"
    "movl %2,%%ebx\n"
    "addl (%%ebx),%%eax\n"
    "adcl $0,%%edx\n"
    "movl %3,%%esi\n"
    "addl (%%esi),%%eax\n"
    "adcl $0,%%edx\n"
    "movl %%eax,(%%esi)\n"
    "movl %%edx,(%%ebx)\n"
    :
    : "m"(a),"m"(b),"m"(c),"m"(rp)
    : "eax","ebx","esi","memory"
    );

}
#endif
#else

mr_small muldiv(mr_small a, mr_small b, mr_small c, mr_small m, mr_small *rp) {
  mr_small q;
  mr_large ldres, dble = (mr_large) a * b + c;
  q = (mr_small) MR_LROUND(dble / m);
  *rp = (mr_small) (dble - (mr_large) q * m);
  return q;
}

#ifdef MR_FP_ROUNDING

mr_small imuldiv(mr_small a,mr_small b,mr_small c,mr_small m,mr_large im,mr_small *rp)
{
    mr_small q;
    mr_large ldres,dble=(mr_large)a*b+c;
    q=(mr_small)MR_LROUND(dble*im);
    *rp=(mr_small)(dble-(mr_large)q*m);
    return q;
}

#endif


#ifndef MR_NOFULLWIDTH

mr_small muldvm(mr_small a, mr_small c, mr_small m, mr_small *rp) {
  mr_small q;
  union doubleword dble;
  dble.h[MR_BOT] = c;
  dble.h[MR_TOP] = a;
  q = (mr_small) (dble.d / m);
  *rp = (mr_small) (dble.d - (mr_large) q * m);
  return q;
}

#ifndef muldvd

mr_small muldvd(mr_small a, mr_small b, mr_small c, mr_small *rp) {
  union doubleword dble;
  dble.d = (mr_large) a * b + c;
  *rp = dble.h[MR_BOT];
  return dble.h[MR_TOP];
}
#endif
#ifndef muldvd2
void muldvd2(mr_small a, mr_small b, mr_small *c, mr_small *rp) {
  union doubleword dble;
  dble.d = (mr_large) a * b + *c + *rp;
  *rp = dble.h[MR_BOT];
  *c = dble.h[MR_TOP];
}
#endif
#endif
#endif