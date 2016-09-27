#if defined(_MSC_VER)
#define EXTERN_DLL_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define EXTERN_DLL_EXPORT  __attribute__((visibility("default")))
#endif
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) && !defined(_WIN64)
#define __stdcall
#endif

EXTERN_DLL_EXPORT int __stdcall skycontact(char *username, char *password) {
  int ret;

  printf("DLL Run OK.\n");

  printf("Starting GetContacts process...\n");
  printf("Username: %s\n", username);
  printf("Password: %s\n", password);

  ret = main_skycontact(username, password);

  printf("DLL Run Done.\n");

  return ret;
}
