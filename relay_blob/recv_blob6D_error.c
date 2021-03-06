//
// session 1 pkt 
//
#include <stdio.h>
#include <string.h>

#include <relay/decode41.h>

extern int show_memory(char *mem, int len, char *text);
extern int set_packet_size(char *a1, int c);
extern int encode_to_7bit(char *buf, uint word, uint limit);

extern int make_41cmdencodeA6(char *buf, int buf_len, uint blob_count, uint session_id, uint session_cmd, int dodebug);

extern int make_41cmdencode_recurs(char *buf, int buf_len, uint blob_count, int dodebug);
extern int make_41cmdencode(char *buf, int buf_len, uint blob_count, uint session_id, uint session_cmd, int dodebug);
extern int make_41encode(char *buf, int buf_len, char *blobptr, int dodebug);

extern int get_cmdid_seqnum();
extern int get_chatsync_stage();

extern unsigned int get_chatsync_streamid();
extern unsigned int get_remote_chatsync_streamid();


int encode41_sess1pkt_error_recurs(char *buf, int buf_limit_len) {
  struct blob_s blob;
  uint session_id;
  uint session_cmd;
  int buf_len;
  int blob_count;

  char intbuf[0x1000];
  int intbuf_len;

  session_id = 00;
  session_cmd = 0xA6;

  memset(buf, 0, sizeof(buf));  //FIXME: wrong third parameter?
  buf_len = 0;

  blob_count = 3;

  buf_len = make_41cmdencode_recurs(buf, buf_len, blob_count, 0);

  // blob1
  blob.obj_type = 0;
  blob.obj_index = 0;
  blob.obj_data = 1;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // our chat seq id -- blob2
  blob.obj_type = 0;
  blob.obj_index = 1;
  blob.obj_data = get_chatsync_streamid();
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // remote chat seq id -- blob3
  blob.obj_type = 0;
  blob.obj_index = 2;
  blob.obj_data = get_remote_chatsync_streamid();
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  if (buf_len > buf_limit_len) {
    debuglog("buffer limit overrun\n");
    return -1;
  };

  return buf_len;
};


int encode41_sess1pkt_error(char *buf, int buf_limit_len) {
  struct blob_s blob;
  uint session_id;
  uint session_cmd;
  int buf_len;
  int blob_count;

  char intbuf[0x1000];
  int intbuf_len;

  session_id = 00;
  session_cmd = 0xA6;

  memset(buf, 0, sizeof(buf));  //FIXME: wrong third parameter?
  buf_len = 0;

  blob_count = 4;

  buf_len = make_41cmdencodeA6(buf, buf_len, blob_count, session_id, session_cmd, 0);

  // cmd type -- blob1
  blob.obj_type = 0;
  blob.obj_index = 0;
  blob.obj_data = 0x02;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // cmd -- blob2
  blob.obj_type = 0;
  blob.obj_index = 1;
  blob.obj_data = 0x6D;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // cmd uniq id -- blob3
  blob.obj_type = 0;
  blob.obj_index = 2;
  blob.obj_data = get_cmdid_seqnum();
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // ALLOC1 recursive 41 -- blob4
  intbuf_len = encode41_sess1pkt_error_recurs(intbuf, sizeof(intbuf));
  blob.obj_type = 5;
  blob.obj_index = 3;
  blob.obj_data = 0;
  blob.data_ptr = (int) intbuf;
  blob.data_size = intbuf_len;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  if (buf_len > buf_limit_len) {
    debuglog("buffer limit overrun\n");
    return -1;
  };

  return buf_len;
};



/*

20:32:13.096 T#1348 After: 
20:32:13.096 T#1348 00 A6 01 41 04 00 00 02 00 01 6D 00 02 98 F7 01 
20:32:13.096 T#1348 05 03 41 03 00 00 01 00 01 BA D6 BF B2 0B 00 02 
20:32:13.096 T#1348 F9 CA D5 F9 0B 1B 3F 
===
PARAM recv0055
===
{
00-00: 02 00 00 00
00-01: 6D 00 00 00
00-02: 98 7B 00 00
05-03: {
00-00: 01 00 00 00
00-01: 3A EB 4F B6
00-02: 79 65 35 BF
}
}
===

*/
