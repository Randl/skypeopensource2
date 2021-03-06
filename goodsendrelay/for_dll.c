#include <tchar.h>
#include <wchar.h>

#if defined(_MSC_VER)
#define EXTERN_DLL_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define EXTERN_DLL_EXPORT  __attribute__((visibility("default")))
#endif
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) && !defined(_WIN64)
#define __stdcall
#endif

EXTERN_DLL_EXPORT int __stdcall relaysend(char *static_myip,
                                          char *static_username,
                                          char *static_uservcard,
                                          wchar_t *static_msg) {
  int ret;

  debuglog_err("DLL Run OK.\n");

  debuglog_err("Starting RelaySend process...\n");

  /*
  debuglog("UserName: %s\n", static_username);
  debuglog("UserVCard: %s\n", static_uservcard);
  debuglog("MSG: %s\n", static_msg);
  */

  //ret = relaysend_main("notnowagainplease","0xe03e31ae403ae012-s-s65.55.223.25:40021-r95.52.236.102:57608-l192.168.1.75:57608", "This is skype msg, lol.");

  ret = relaysend_main(static_myip, static_username, static_uservcard, static_msg);

  debuglog_err("DLL Run Done.\n");

  return ret;
}


EXTERN_DLL_EXPORT int __stdcall multysend(char *static_userip, char *static_msg) {
  int ret;

  debuglog_err("DLL Run OK.\n");

  debuglog_err("Starting MultySend process...\n");
  debuglog_err("UserIP: %s\n", static_userip);
  debuglog_err("MSG: %s\n", static_msg);

  //ret = multysend_main("xot_iam:192.168.1.110:5322", "This is skype msg, lol.");

  ret = multysend_main(static_userip, static_msg);

  debuglog_err("DLL Run Done.\n");

  return ret;
}

