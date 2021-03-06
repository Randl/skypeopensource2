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


//
// sess1pkt 6D req (stage inc)
// (send0014)
//
int encode41_sess1pkt4_recurs(char *buf, int buf_limit_len) {
  struct blob_s blob;
  int buf_len;
  int blobs_count;
  memset(buf, 0, sizeof(buf));  //FIXME: wrong third parameter?
  buf_len = 0;

  blobs_count = 3;

  buf_len = make_41cmdencode_recurs(buf, buf_len, blobs_count, 0);

  // 00-01 our chat seq id -- blob1
  blob.obj_type = 0;
  blob.obj_index = 1;
  blob.obj_data = get_chatsync_streamid();
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // 00-03 stage0 -- blob2
  blob.obj_type = 0;
  blob.obj_index = 3;
  blob.obj_data = get_chatsync_stage();
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // 00-08 info -- blob3
  blob.obj_type = 0;
  blob.obj_index = 8;
  blob.obj_data = 0x01;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  if (buf_len > buf_limit_len) {
    debuglog("buffer limit overrun\n");
    return -1;
  };

  return buf_len;
};


int encode41_sess1pkt_req(char *buf, int buf_limit_len, char *chatstr) {
  struct blob_s blob;
  uint session_id;
  uint session_cmd;
  int buf_len;
  int blob_count;

  char intbuf[0x1000];
  int intbuf_len;

  session_id = 00;
  session_cmd = 0xA6;

  blob_count = 4;

  memset(buf, 0, sizeof(buf));  //FIXME: wrong third parameter?
  buf_len = 0;
  buf_len = make_41cmdencodeA6(buf, buf_len, blob_count, session_id, session_cmd, 0);

  // type of session_cmd -- blob1
  blob.obj_type = 0;
  blob.obj_index = 0;
  blob.obj_data = 0x02;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // session_cmd command -- blob2
  blob.obj_type = 0;
  blob.obj_index = 1;
  blob.obj_data = 0x6D;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // session_cmd uniq_send_id -- blob3
  blob.obj_type = 0;
  blob.obj_index = 2;
  //blob.obj_data = 0x8F93;
  blob.obj_data = get_cmdid_seqnum();
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // ALLOC1 recursive 00-19 info -- blob4
  intbuf_len = encode41_sess1pkt4_recurs(intbuf, sizeof(intbuf));
  blob.obj_type = 5;
  blob.obj_index = 0x03;
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

sess1pkt stage request

20:32:12.284 T#1348 Logger AES dump from 00914C97 len 23
20:32:12.284 T#1348 Before: 
20:32:12.284 T#1348 00 A6 01 41 04 00 00 02 00 01 6D 00 02 A4 96 03 
20:32:12.284 T#1348 05 03 41 03 00 01 F9 CA D5 F9 0B 00 03 01 00 08 
20:32:12.284 T#1348 01 DA E4 
===
PARAM send0018
===
{
00-00: 02 00 00 00
00-01: 6D 00 00 00
00-02: 24 CB 00 00
05-03: {
00-01: 79 65 35 BF
00-03: 01 00 00 00
00-08: 01 00 00 00
}
}
===

*/

