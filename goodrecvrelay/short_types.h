#ifndef _SHORT_TYPES_SKYPEOPENSOURCE2
#define _SHORT_TYPES_SKYPEOPENSOURCE2 

#include <stdint.h>

#ifndef uint
#define uint uint_fast32_t  //TODO
#endif

#ifndef u8
#define u8   uint_fast8_t
#endif
#ifndef u16
#define u16  uint_fast16_t
#endif
#ifndef u32
#define u32  uint_fast32_t
#endif
#ifndef u64
#define u64  uint_fast64_t
#endif

#endif
