//
// Created by eabes on 07.09.2016.
//
#include <stdio.h>
#include <string.h>

#include <relay/decode41.h>


extern int show_memory(char *mem, int len, char *text);
extern int set_packet_size(char *a1, int c);
extern int encode_to_7bit(char *buf, uint word, uint limit);

extern int make_41cmdencode_recurs(char *buf, int buf_len, uint blob_count, int dodebug);
extern int make_41cmdencode(char *buf, int buf_len, uint blob_count, uint session_id, uint session_cmd, int dodebug);
extern int make_41encode(char *buf, int buf_len, char *blobptr, int dodebug);


extern u8 MSG_TEXT[0x100];
extern u8 CHAT_STRING[0x100];
extern u8 CHAT_PEERS[0x100];
extern u8 CREDENTIALS[0x105];
extern uint CREDENTIALS_LEN;
extern u8 NEWBLK[0x1000];
extern uint NEWBLK_LEN;
extern u8 REMOTE_NAME[0x100];

extern u8 LOCALNODE_VCARD[0x1B];

extern uint BLOB_0_5;
extern uint BLOB_0_5__1;
extern uint BLOB_0_6;
extern uint BLOB_0_7__2;
extern uint BLOB_0_7__3;
extern uint BLOB_0_7__4;
extern uint BLOB_0_A__2;
extern uint BLOB_0_A__3;


extern uint HEADER_ID_REMOTE_LAST;
extern uint HEADER_ID_SEND;


extern uint START_HEADER_ID;
extern uint global_unknown_cmd24_signed_id;
extern uint global_unknown_cmd2A_signed_id;

extern uint global_msg_time_sec;
extern uint global_msg_time_min;

extern uint global_unknown_cmd24_time;
extern uint global_unknown_cmd24_time_sec;
extern uint global_unknown_cmd24_blob1b;


int encode41_newblk4(char *buf, int buf_limit_len) {
  struct blob_s blob;
  int buf_len;
  int blob_count;

  memset(buf, 0, sizeof(buf));  //FIXME: wrong third parameter?
  buf_len = 0;

  blob_count = 9;

  buf_len = make_41cmdencode_recurs(buf, buf_len, blob_count, 0);

  // seq id NEWBLK3 for sha-1 hashing -- blob1
  blob.obj_type = 0;
  blob.obj_index = 0;
  blob.obj_data = 4;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // LOCALNODE_VCARD -- blob2
  blob.obj_type = 4;
  blob.obj_index = 3;
  blob.obj_data = 0;
  blob.data_ptr = (int) LOCALNODE_VCARD;
  blob.data_size = sizeof(LOCALNODE_VCARD);
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // unix timestamp in seconds -- blob3
  blob.obj_type = 0;
  blob.obj_index = 5;
  //blob.obj_data = time(NULL);
  blob.obj_data = global_unknown_cmd24_time_sec;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // unix timestamp in minutes -- blob4
  blob.obj_type = 0;
  blob.obj_index = 6;
  //blob.obj_data = time(NULL) / 60;
  blob.obj_data = global_unknown_cmd24_time;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // START_HEADER_ID  -- blob5
  blob.obj_type = 0;
  blob.obj_index = 7;
  //blob.obj_data = START_HEADER_ID + 1;
  blob.obj_data = START_HEADER_ID;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // clear chat text data -- blob6
  blob.obj_type = 3;
  blob.obj_index = 0x0E;
  blob.obj_data = 0;
  //blob.data_ptr = (int)MSG_TEXT;
  //blob.data_size = strlen(MSG_TEXT)+1;
  blob.data_ptr = (int) REMOTE_NAME;
  blob.data_size = strlen(REMOTE_NAME) + 1;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // some flag -- blob7
  blob.obj_type = 0;
  blob.obj_index = 0x0F;
  //blob.obj_data = 0x0174;
  blob.obj_data = global_unknown_cmd24_blob1b;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // header from cmd24 -- blob8
  blob.obj_type = 0;
  blob.obj_index = 0x0A;
  //blob.obj_data = 0xC598D521;
  blob.obj_data = global_unknown_cmd24_signed_id;
  blob.data_ptr = 0;
  blob.data_size = 0;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  // some flag -- blob9
  blob.obj_type = 6;
  blob.obj_index = 0x1B;
  blob.obj_data = 0x00;
  blob.data_ptr = 0;
  blob.data_size = 1;
  buf_len = make_41encode(buf, buf_len, (char *) &blob, 0);

  if (buf_len > buf_limit_len) {
    debuglog("buffer limit overrun\n");
    return -1;
  };

  return buf_len;
};

