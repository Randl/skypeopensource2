//
// Routines to deal with CharSet.Unicode in csharp 
// for get russian symbols in utf8
//

#include <stdio.h>

#include <string.h>
#include <wchar.h>

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

// utf8 -- MultiByte
// unicode -- WideChar
int Utf8ToUnicode(wchar_t *wcsString, char *mbString, int max_len) {
  int need_len;

  need_len = MultiByteToWideChar(CP_UTF8, 0, mbString, -1, wcsString, 0);

  if (need_len > max_len) {
    debuglog("Buffer overrun at Utf8ToUnicode\n");
    return -1;
  };

  MultiByteToWideChar(CP_UTF8, 0, mbString, -1, wcsString, need_len);

  show_memory_with_ascii(wcsString, 0x20, "wcsString:");

  return 1;
};


// utf8 -- MultiByte
// unicode -- WideChar
int UnicodeToUtf8(char *mbString, wchar_t *wcsString, int maxlen) {

  WideCharToMultiByte(CP_UTF8, 0, wcsString, -1, mbString, maxlen, NULL, NULL);

  show_memory_with_ascii(mbString, 0x20, "mbString:");

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

