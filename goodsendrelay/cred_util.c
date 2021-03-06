//
//
//

#include <stdio.h>

#include <time.h>

#include <util/short_types.h>

extern int show_memory(char *mem, int len, char *text);

extern int get_04_03_blob(u8 *buf, int len, u8 *membuf, int *membuf_len);
extern int get_04_04_blob(u8 *buf, int len, u8 *membuf, int *membuf_len);

extern int get_00_01_blob(u8 *buf, int buf_len, int *chat_cmd);
extern int get_00_02_blob(u8 *buf, int buf_len, int *uic_crc);
extern int get_00_04_blob(u8 *buf, int buf_len, int *expire_time);
extern int get_00_09_blob(u8 *buf, int buf_len, int *created_time);

extern int rsa_unsign_cred(char *buf, int len, char *outbuf);


// check if credentials expired
int do_time_check(char *outbuf, int outbuf_len) {
  int expire_time;
  int created_time;
  time_t lexpire_time;
  time_t lcreated_time;
  struct tm *newtime;
  time_t lcurrent_time;

  debuglog("\n");

  // add to get blob of Expire time:
  get_00_04_blob(outbuf, 0x100, &expire_time);
  expire_time = expire_time * 60;
  lexpire_time = expire_time;
  debuglog("Cert expire time: %d\n", lexpire_time);
  newtime = localtime(&lexpire_time);
  debuglog("%s\n", asctime(newtime));

  // add to get blob of Created Time:
  get_00_09_blob(outbuf, 0x100, &created_time);
  // *60 for get seconds,
  // and minus year for unknown reason...
  created_time = created_time * 60 - 31536000;
  lcreated_time = created_time;
  debuglog("Cert created time: %d\n", lcreated_time);
  newtime = localtime(&lcreated_time);
  debuglog("%s\n", asctime(newtime));

  // checking current time
  time(&lcurrent_time);
  if (lcurrent_time > lexpire_time) {
    debuglog("Certificate expired!\n");
    debuglog("(!!!)\n");
    return -1;
  };

  debuglog("\n");

  return 0;
};


int decode_profile_for_time_check(char *credentials, int credentials_len) {
  u8 outbuf[0x100];
  int i;
  int ret;

  debuglog("Unsigning first 0x100 bytes...\n");
  rsa_unsign_cred(credentials + 4, 0x100, outbuf);
  show_memory(outbuf, 0x100, "unsign cred:");

  /*
  debuglog("Extracting pubkey from certificate (credentials)\n");
  // save remote user pubkey
  for(i=0;i<0x100;i++){
      if((outbuf[i]==0x80) && (outbuf[i+1]==0x01)){
          memcpy(user_pubkey,outbuf+i+2,0x80);
      };
  };

  show_memory(user_pubkey,0x80,"user pubkey data:");
  */


  debuglog("decoded credentials:\n");
  main_unpack(outbuf, 0x100);

  ret = do_time_check(outbuf, 0x100);

  return ret;
};

