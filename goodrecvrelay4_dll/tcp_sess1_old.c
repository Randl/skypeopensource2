/*  
*
* Direct TCP connect to skype client
* cmd 109 session
*
*/

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <io.h>

// for rc4
//#include "Expand_IV.h"

#include <skype/skype_rc4.h>

// for aes
#include <crypto/rijndael.h>

// for 41 
#include "decode41.h"

//#include "defs.h"


// rc4 obfuscation

//extern void Skype_RC4_Expand_IV (RC4_context * const rc4, const u32 iv, const u32 flags);
//extern void RC4_crypt (u8 * buffer, u32 bytes, RC4_context * const rc4, const u32 test);


extern int Calculate_CRC32_For41(char *a2, int a3);
extern unsigned int Calculate_CRC32(char *crc32, int bytes);

// socket comm
extern int udp_talk(char *remoteip, unsigned short remoteport, char *buf, int len, char *result);
extern int tcp_talk(char *remoteip, unsigned short remoteport, char *buf, int len, char *result, int need_close);
extern int tcp_talk_recv(char *remoteip, unsigned short remoteport, char *result, int need_close);
extern int tcp_talk_send(char *buf, int len);


// sha1 and rsa crypto function
extern int _get_sha1_data(char *buf, int len, char *outbuf, int need_convert);
extern int _get_sign_data(char *buf, int len, char *outbuf);
//extern int _get_unsign_cred(char *buf, int len, char *outbuf);

// utils
extern int get_blkseq(char *data, int datalen);
extern int show_memory(char *mem, int len, char *text);
extern int get_packet_size(char *data, int len);
extern int decode41(char *data, int len, char *text);
extern int set_packet_size(char *a1, int c);
extern int process_aes(char *buf, int buf_len, int usekey, int blkseq, int need_xor);
extern int first_bytes_correction(char *header, int header_len, char *buf, int buf_len);

//blobs encode 
int encode41_sesspkt_ack(char *buf, int buf_limit_len, uint cmd);

extern int encode41_sess1pkt1(char *buf, int buf_limit_len);
extern int encode41_sess1pkt2(char *buf, int buf_limit_len);
extern int encode41_sess1pkt3(char *buf, int buf_limit_len);
extern int encode41_sess1pkt4(char *buf, int buf_limit_len);
extern int encode41_sess1pkt5(char *buf, int buf_limit_len);
extern int encode41_sess1pkt6(char *buf, int buf_limit_len);
extern int encode41_sess1pkt7(char *buf, int buf_limit_len);
extern int encode41_sess1pkt8(char *buf, int buf_limit_len);

extern int encode41_sess1pkt_cmd24(char *buf, int buf_limit_len);
extern int encode41_sess1pkt_cmd2A(char *buf, int buf_limit_len);
extern int encode41_sess1pkt_cmd13(char *buf, int buf_limit_len);

extern int encode41_newblk1(char *buf, int buf_limit_len);
extern int encode41_newblk2(char *buf, int buf_limit_len);
extern int encode41_newblk3(char *buf, int buf_limit_len);

// global data

extern RC4_context rc4_send;
extern RC4_context rc4_recv;

extern u8 challenge_response[0x80];

extern u8 aes_key[0x20];
extern u32 remote_session_id;
extern u32 LOCAL_SESSION_ID;

extern u32 confirm[0x100];
extern u32 confirm_count;


extern u8 MSG_TEXT[0x100];
extern u8 CHAT_STRING[0x100];
extern u8 CHAT_PEERS[0x100];
extern u8 CREDENTIALS[0x105];
extern uint CREDENTIALS_LEN;
extern u8 NEWBLK[0x1000];
extern uint NEWBLK_LEN;
extern u8 REMOTE_NAME[0x100];

extern u8 REMOTE_CHAT_STRING[0x100];

extern uint HEADER_ID_REMOTE_LAST;
extern uint HEADER_ID_SEND;
extern uint HEADER_ID_SEND_CRC;

extern uint START_HEADER_ID;


extern uint NEWSESSION_FLAG;
extern uint NO_HEADERS_FLAG;

extern uint DEBUG_RC4;

extern uint global_chatsync_stage;

extern u8 RECV_CHAT_COMMANDS[0x100];
extern uint RECV_CHAT_COMMANDS_LEN;

// global aes blkseq key
extern int blkseq;

extern u8 CHAT_PEERS_REVERSED[0x100];


unsigned int make_tcp_client_cmd13r_sendheaders() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd13r(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd13r");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd13r_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_sendheaders() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  debuglog("\n\n\n");

  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd13one(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd13one");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd13one_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_sendheaders2() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  debuglog("\n\n\n");

  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd13(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd13");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd13_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_msgbody(char *ip, unsigned short port) {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  debuglog("\n\n\n");

  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd2B(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd2B");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd2B_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_chatend() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd0C(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd0C_chatend");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd0C_chatend_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_sendend() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd13end(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd13end");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd13end_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_unkheader() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd10(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd10");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd10_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_cmd15() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  debuglog("\n\n\n");

  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd15(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd15");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd15_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};

/*
unsigned int make_tcp_client_cmd0F_chatok() {
	int len;
	char *pkt;

	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;


	debuglog("\n\n\n");

	memset(buf1,0,sizeof(buf1));
  	buf1_len=encode41_sess1pkt_cmd0F_chatok(buf1, sizeof(buf1));
	show_memory(buf1, buf1_len, "sess1pkt_cmd0F_chatok");

	main_unpack(buf1, buf1_len);

	//aes encrypt block 1
	blkseq=blkseq+1;
	buf1_len=process_aes(buf1, buf1_len, 1, blkseq, 0);


	// first bytes correction
	// calculate for 4 and 5 byte fixing
	buf1header_len=first_bytes_correction(buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess1pkt_cmd0F_chatok_header");


	// assembling pkt for sending
	pkt=(char *)malloc(0x1000);
	memset(pkt,0,0x1000);
	len=0;

	memcpy(pkt,buf1header,buf1header_len);
	len=len+buf1header_len;
	
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	

	// RC4 encrypt pkt
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	

	// display pkt
	show_memory(pkt, len, "Send pkt");

	// send pkt
	len=tcp_talk_send(pkt,len);
	if (len<=0) {
		debuglog("send err\n");
		return -1;
	};

	return 0;
};
*/


unsigned int make_tcp_client_cmd1D_request_uic() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd1D(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd1D");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd1D_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_cmd27_finish() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd27(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd27");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd27_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_7B(char *ip, unsigned short port) {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_7B(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_7B");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_7B_header");

  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };


  return 0;
};


unsigned int make_tcp_client_sess1_send_58(char *ip, unsigned short port) {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_58(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_58");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_58_header");

  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };


  return 0;
};


unsigned int make_tcp_client_sess1_send_53(char *ip, unsigned short port) {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_53(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_53");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_53_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  if (DEBUG_RC4) { show_memory(pkt, len, "Before RC4 encrypt"); };
  RC4_crypt(pkt, len, &rc4_send, 0);
  if (DEBUG_RC4) { show_memory(pkt, len, "After RC4 encrypt"); };

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };


  return 0;
};


unsigned int make_tcp_client_sess1_send_chatinit() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd0D(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd0D_chatinit");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd0D_chatinit_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_chatsign() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd24(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd24chatsign");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd24chatsign_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


//
// Sending "headers with sign" packet, newblk2 
// 
unsigned int make_tcp_client_sess1_send_headsign(char *ip, unsigned short port) {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_cmd2A(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_cmd2Achatsign");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_cmd2Aheadsign_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };

  return 0;
};


unsigned int make_tcp_client_sess1_send_7D() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_7D(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_7D");

  main_unpack(buf1, buf1_len);


  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_7D_header");


  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };


  return 0;
};


unsigned int make_tcp_client_sess1_send_7A() {
  int len;
  char *pkt;

  u8 buf1[0x1000];
  int buf1_len;
  u8 buf1header[0x10];
  int buf1header_len;


  memset(buf1, 0, sizeof(buf1));
  buf1_len = encode41_sess1pkt_7A(buf1, sizeof(buf1));
  show_memory(buf1, buf1_len, "sess1pkt_7A");

  main_unpack(buf1, buf1_len);

  //aes encrypt block 1
  blkseq = blkseq + 1;
  buf1_len = process_aes(buf1, buf1_len, 1, blkseq, 0);


  // first bytes correction
  // calculate for 4 and 5 byte fixing
  buf1header_len = first_bytes_correction(buf1header, sizeof(buf1header) - 1, buf1, buf1_len);
  show_memory(buf1header, buf1header_len, "sess1pkt_7A_header");

  // assembling pkt for sending
  pkt = (char *) malloc(0x1000);
  memset(pkt, 0, 0x1000);
  len = 0;

  memcpy(pkt, buf1header, buf1header_len);
  len = len + buf1header_len;

  memcpy(pkt + len, buf1, buf1_len);
  len = len + buf1_len;


  // RC4 encrypt pkt
  show_memory(pkt, len, "Before RC4 encrypt");
  RC4_crypt(pkt, len, &rc4_send, 0);
  show_memory(pkt, len, "After RC4 encrypt");

  // display pkt
  show_memory(pkt, len, "Send pkt");

  // send pkt
  len = tcp_talk_send(pkt, len);
  if (len <= 0) {
    debuglog("send err\n");
    return -1;
  };


  return 0;
};


