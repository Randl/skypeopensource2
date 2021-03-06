//
// Routines to deal with CharSet.Unicode in csharp 
// for get russian symbols in utf8
//

#include <wchar.h>

#include <stdio.h>

#include <string.h>
// windows specific
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WIN64)
#include <windows.h>
#else
  #define DWORD u32
#endif

#ifndef CP_UTF8
  #define CP_UTF8 65001
#endif

extern int show_memory(char *mem, int len, char *text);

int UnicodeToUtf8(char *mbString, wchar_t *wcsString, int maxlen) {

  WideCharToMultiByte(CP_UTF8, 0, wcsString, -1, mbString, maxlen, NULL, NULL);

  show_memory_with_ascii(mbString, 0x10, "mbString:");

  return 1;
};


int UnicodeToAscii(char *szAscii, wchar_t *szUnicode) {
  int len, i;

  if ((szUnicode == NULL) || (szAscii == NULL))
    return 0;

  len = wcslen(szUnicode);

  for (i = 0; i < len + 1; i++) {
    *szAscii++ = (char) (*szUnicode++);
  };

  return 1;
};


////////////////////////////
////////////////////////////
////////////////////////////


/*
// with memory alloc for char string
int UnicodeToUtf8(char *mbString, wchar_t *wcsString) {
	int sizeRequired = 0;

    //nlen = wcslen(static_msg);
    //debuglog("nlen = %d\n", nlen);

    // count size of string of chars
    sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wcsString, -1, NULL, 0,  NULL, NULL);

    mSstring = malloc(sizeRequired);

    debuglog("Bytes need for UTF8 encoding (with NUL terminator): %d\n", sizeRequired);

    WideCharToMultiByte( CP_UTF8, 0, wcsString, -1, mbString, sizeRequired,  NULL, NULL);

    show_memory_with_ascii(mbString, 0x10, "mbString:");

    return 1;
};
*/
