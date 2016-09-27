// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:// windows specific
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WIN64)
#include <windows.h>
#else
  #define DWORD u32
#endif



// TODO: reference additional headers your program requires here
