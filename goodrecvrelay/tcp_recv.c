//
// tcp recv
//

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
//#include <io.h>


#include <skype/skype_rc4.h>

// for aes
#include <crypto/rijndael.h>

// for 41 
#include <relay/decode41.h>


extern int Calculate_CRC32_For41(char *a2, int a3);
extern unsigned int Calculate_CRC32(char *crc32, int bytes);


// sha1 and rsa crypto function
extern int _get_sha1_data(char *buf, int len, char *outbuf, int need_convert);
extern int _get_decode_data(char *buf, int len, char *outbuf);
extern int _get_sign_data(char *buf, int len, char *outbuf);
extern int _get_unsign_cred(char *buf, int len, char *outbuf);
extern int _get_encode_data(char *buf, int len, char *outbuf);

// utils
extern int process_aes_crypt(char *data, int datalen, int usekey, int blkseq, int need_xor);
extern int show_memory(char *mem, int len, char *text);
extern int get_packet_size(char *data, int len);
extern int decode41(char *data, int len, char *text);
extern int process_aes(char *buf, int buf_len, int usekey, int blkseq, int need_xor);
extern int first_bytes_correction(char *header, int header_len, char *buf, int buf_len);
extern int get_blkseq(char *data, int datalen);
extern int get_packet_size2(char *data, int len, int *header_len);
extern int get_packet_size3(char *data, int len, int *header_len);


// 41 decode
extern int main_unpack_checkblob(u8 *indata, u32 inlen, int type, int id);
extern int main_unpack_getbuf(u8 *indata, u32 inlen, u8 *membuf, int *membuf_len, int type, int id);
extern int main_unpack(u8 *indata, u32 inlen);


extern int set_remote_chatsync_streamid(unsigned int data_int);
extern int make_tcp_client_set_chatinit(char *tmpbuf, int cnt);

extern int recovery_signed_data(char *buf41, int buf41_len);

// global data
extern RC4_context rc4_send;
extern RC4_context rc4_recv;


extern u8 REMOTE_AUTHORIZED188[0x189];
extern uint REMOTE_AUTHORIZED188_LEN;
extern u8 CREDENTIALS2_HASH[0x15];
extern u8 AFTER_CRED2[0x81];

extern u8 CREDENTIALS[0x105];
extern uint CREDENTIALS_LEN;


extern u8 CHALLENGE_RESPONSE[0x80];
extern u8 LOCAL_NONCE[0x80];

extern u32 REMOTE_SESSION_ID;
extern u32 LOCAL_SESSION_ID;

extern u8 remote_credentials[0x100];
extern u8 remote_pubkey[0x80];
extern u8 aes_key[0x20];

extern u32 confirm[0x100];
extern u32 confirm_count;

extern uint HEADER_ID_REMOTE_FIRST;
extern uint HEADER_ID_REMOTE_LAST;
extern uint HEADER_ID_SEND;
extern uint HEADER_ID_SEND_CRC;

extern uint NEWSESSION_FLAG;
extern uint NO_HEADERS_FLAG;

extern uint HEADER_SLOT_CRC1;
extern uint HEADER_SLOT_CRC2;

extern uint GOT_REMOTE_MSG_COUNT;

extern u8 RECV_CHAT_COMMANDS[0x100];
extern uint RECV_CHAT_COMMANDS_LEN;

extern int GLOBAL_STATE_MACHINE;

extern enum { AES_KEY_INIT, AES_KEY_OK };

extern uint global_unknown_cmd24_signed_id;
extern uint global_unknown_cmd24_time;

extern uint global_unknown_cmd24_blob1b;

extern uint HEADER_ID_CMD27_CRC;
extern uint HEADER_ID_CMD27_ID;

extern uint HEADER_ID_CMD13R3_ID;
extern uint HEADER_ID_CMD13R4_ID;

extern uint HEADER_ID_LOCAL_FIRST;
extern uint HEADER_ID_LOCAL_LAST;


extern char *RECV_MANYMSG_BUF;
extern uint RECV_MANYMSG_BUF_LEN;


extern uint RECV_MANYMSG_00_06_size_total;
extern uint RECV_MANYMSG_00_06_size_recv;
extern uint RECV_MANYMSG_00_06_flag;

extern int newchatinit_flag;
extern int restorechat_flag;

extern int not_aes_counter;

unsigned int check_commands_array(unsigned int cmd) {
  unsigned int i = 0;

  for (i = 0; i < RECV_CHAT_COMMANDS_LEN; i++) {

    debuglog("i=%d, CMD=0x%08X\n", i, RECV_CHAT_COMMANDS[i]);

    if (RECV_CHAT_COMMANDS[i] == cmd) {

      debuglog("I: 0x%08X\n", i);
      debuglog("CMD: 0x%08X\n", cmd);

      show_memory(RECV_CHAT_COMMANDS, RECV_CHAT_COMMANDS_LEN, "RECV_CHAT_COMMANDS (before):");
      memcpy(RECV_CHAT_COMMANDS, RECV_CHAT_COMMANDS + i, RECV_CHAT_COMMANDS_LEN - i);
      show_memory(RECV_CHAT_COMMANDS, RECV_CHAT_COMMANDS_LEN, "RECV_CHAT_COMMANDS (after):");

      // zero-ing rest of buffer
      RECV_CHAT_COMMANDS_LEN = RECV_CHAT_COMMANDS_LEN - i;
      memset(RECV_CHAT_COMMANDS + RECV_CHAT_COMMANDS_LEN, 0x00, 0x100 - RECV_CHAT_COMMANDS_LEN);

      show_memory(RECV_CHAT_COMMANDS, RECV_CHAT_COMMANDS_LEN, "RECV_CHAT_COMMANDS (after zeroing):");

      return 1;
    };
  };

  return 0;
};


//
// calculate buddy_authorize_cert2
//

unsigned int buddy_authorize_cert2() {
  uint UIC_CRC2;

  memcpy(REMOTE_AUTHORIZED188 + 4, CREDENTIALS, CREDENTIALS_LEN);

  UIC_CRC2 = Calculate_CRC32((char *) REMOTE_AUTHORIZED188 + 0x04, 0x104);
  debuglog("UIC_CRC2 (remote cert crc) = %08X\n", UIC_CRC2);

  /////////////////////
  // SHA1 hash
  /////////////////////
  //make hash of CREDENTIALS 0x104 bytes.
  //save to CREDENTIALS_HASH
  if (1) {
    char *buf;
    char *outbuf;

    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);

    //prepare data for hashing
    memcpy(buf, CREDENTIALS, CREDENTIALS_LEN);

    //print it
    show_memory(buf, CREDENTIALS_LEN, "SHA1 input");

    //make sha1 hash
    _get_sha1_data(buf, CREDENTIALS_LEN, outbuf, 1);

    //print it
    show_memory(outbuf, 0x14, "SHA1 output(hash)");

    //copy hash
    memcpy(CREDENTIALS2_HASH, outbuf, 0x14);
  };


  // modify hash
  memcpy(AFTER_CRED2 + 0x35, CREDENTIALS2_HASH, 0x14);
  //modify buddy_authorized

  // not 48???
  memcpy(AFTER_CRED2 + 0x35 + 0x14, "buddy_authorizednotnowagainplease", 33);

  //memcpy(AFTER_CRED2+0x5C,"buddy_authorizedthemagicforyou\x00",31);


  /////////////////////
  // SHA1 hash
  /////////////////////
  //make hash of AFTER_CRED 0x80 bytes.
  //save to CREDENTIALS_HASH
  if (1) {
    char *buf;
    char *outbuf;

    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);

    //prepare data for hashing
    memcpy(buf, AFTER_CRED2 + 0x35, 0x80 - 0x14 - 1 - 0x35);

    //print it
    show_memory(buf, 0x80 - 0x14 - 1 - 0x35, "SHA1 input");

    //make sha1 hash
    _get_sha1_data(buf, 0x80 - 0x14 - 1 - 0x35, outbuf, 1);

    //print it
    show_memory(outbuf, 0x14, "SHA1 output(hash)");

    //copy hash
    memcpy(AFTER_CRED2 + 0x80 - 0x14 - 1, outbuf, 0x14);

  };




  ///////////////////////
  //RSA sign
  ///////////////////////
  //for sign 0x80 byte after credentials
  if (1) {
    char *buf;
    char *outbuf;


    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);


    //copy
    memcpy(buf, AFTER_CRED2, 0x80);

    //before RSA sign-ing
    show_memory(buf, 0x80, "RSA SIGN input");

    //make rsa sign
    _get_sign_data(buf, 0x80, outbuf);

    //copy rsa sign to credentials188 buffer
    memcpy(REMOTE_AUTHORIZED188 + 0x100 + 0x08, outbuf, 0x80);

    //print credentials 0x188
    show_memory(REMOTE_AUTHORIZED188, REMOTE_AUTHORIZED188_LEN, "RSA SIGN AUTHORIZE_188");

  };

  //UIC_CRC2=Calculate_CRC32( (char *)CREDENTIALS,CREDENTIALS_LEN);
  //debuglog("UIC_CRC = %08X\n",UIC_CRC);

  return 0;
};


/*
unsigned int buddy_authorize_cert() {

	//memset(MSG_TEXT,0x42,10*1024-1);
	//MSG_TEXT[10*1024]=0;

	uint UIC_CRC2;

	//memcpy(REMOTE_AUTHORIZED188+0x08,remote_credentials,0x100);
	memcpy(REMOTE_AUTHORIZED188+0x08,remote_credentials,0x100);

	UIC_CRC2=Calculate_CRC32( (char *)REMOTE_AUTHORIZED188+0x04,0x104);
	debuglog("UIC_CRC2 (remote cert crc) = %08X\n",UIC_CRC2);

	/////////////////////
	// SHA1 hash
	/////////////////////
	//make hash of CREDENTIALS 0x104 bytes.
	//save to CREDENTIALS_HASH
	if (1) {
		char *buf;
		char *outbuf;

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		//prepare data for hashing
		memcpy(buf,"\x00\x00\x00\x01",4);
		memcpy(buf+4,remote_credentials,0x100);

		//print it
		show_memory(buf, 0x104, "SHA1 input");

		//make sha1 hash
		_get_sha1_data(buf, 0x104, outbuf, 1);

		//print it
		show_memory(outbuf, 0x14, "SHA1 output(hash)");

		//copy hash
		memcpy(CREDENTIALS2_HASH,outbuf,0x14);

	};


	// modify hash
	memcpy(AFTER_CRED2+0x48,CREDENTIALS2_HASH, 0x14);
	//modify init_unk
	//memcpy(strbuf, "buddy_authorizedthemagicforyou\x00",31);
	memcpy(AFTER_CRED2+0x5C,"buddy_authorizedthemagicforyou\x00",31);

	
	/////////////////////
	// SHA1 hash
	/////////////////////
	//make hash of AFTER_CRED 0x80 bytes.
	//save to CREDENTIALS_HASH
	if (1) {
		char *buf;
		char *outbuf;

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		//prepare data for hashing
		memcpy(buf,AFTER_CRED2+0x48,0x80-0x14-1-0x48);

		//print it
		show_memory(buf, 0x80-0x14-1-0x48, "SHA1 input");

		//make sha1 hash
		_get_sha1_data(buf, 0x80-0x14-1-0x48, outbuf, 1);

		//print it
		show_memory(outbuf, 0x14, "SHA1 output(hash)");

		//copy hash
		memcpy(AFTER_CRED2+0x80-0x14-1,outbuf,0x14);

	};




	///////////////////////
	//RSA sign
	///////////////////////
	//for sign 0x80 byte after credentials
	if (1) {
		char *buf;
		char *outbuf;


		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);


		//copy
		memcpy(buf,AFTER_CRED2,0x80);
		
		//before RSA sign-ing
		show_memory(buf, 0x80, "RSA SIGN input");

		//make rsa sign
		_get_sign_data(buf, 0x80, outbuf);

		//copy rsa sign to credentials188 buffer
		memcpy(REMOTE_AUTHORIZED188+0x100+0x08,outbuf,0x80);

		//print credentials 0x188
		show_memory(REMOTE_AUTHORIZED188, REMOTE_AUTHORIZED188_LEN, "RSA SIGN cred188");

	};

	//UIC_CRC2=Calculate_CRC32( (char *)CREDENTIALS,CREDENTIALS_LEN);
	//debuglog("UIC_CRC = %08X\n",UIC_CRC);

	return 0;
};
*/


int on_recv_44_41(char *pktbuf, int pktlen) {
  u8 sha1[0x14];
  u8 rnd64bit[0x8];


  /////////////////////////////////
  // Process received pkt
  /////////////////////////////////


  // show header
  show_memory(pktbuf, 5, "Header");


  /////////////////////////////
  // 41 decode
  /////////////////////////////
  // for getting remote session id
  // and rnd64bit challenge
  // and pubkey from credentials
  if (1) {
    u8 membuf[0x1000];
    int membuf_len;
    int ret;
    u8 tmpbuf[0x100];
    u8 data_64bit[0x08];
    int kk;
    int data_int;



    // get REMOTE_SESSION_ID
    debuglog("Looking for 00-03 (remote session id) blob...\n");
    ret = main_unpack_checkblob(pktbuf, pktlen, 0x00, 0x03);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      data_int = 0;
      main_unpack_getobj00(pktbuf, pktlen, &data_int, 0x00, 0x03);
      memcpy(&REMOTE_SESSION_ID, &data_int, 4);
      debuglog("remote session id: 0x%08X\n", REMOTE_SESSION_ID);
    } else {
      debuglog("REMOTE_SESSION_ID blob not found\n");
      return -1;
    };


    // get rnd64bit challenge
    debuglog("Looking for 01-09 (rnd64bit challenge) blob...\n");
    ret = main_unpack_checkblob(pktbuf, pktlen, 0x01, 0x09);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      memset(data_64bit, 0, 0x08);
      main_unpack_getobj01(pktbuf, pktlen, data_64bit, 0x01, 0x09);
      memcpy(rnd64bit, data_64bit + 4, 4);
      memcpy(rnd64bit + 4, data_64bit, 4);
      show_memory(rnd64bit, 8, "rnd64bit");
    } else {
      debuglog("RND64bit Challenge blob not found\n");
      return -1;
    };


    debuglog("Looking for 4-5 (remote credentials) blob...\n");
    ret = main_unpack_checkblob(pktbuf, pktlen, 0x04, 0x05);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      main_unpack_getbuf(pktbuf, pktlen, membuf, &membuf_len, 0x04, 0x05);
      if (membuf_len < 0x188) {
        debuglog("credentials size error\n");
        return -1;
      };
      debuglog("MEMBUF_LEN: %d bytes\n", membuf_len);
      show_memory(membuf, membuf_len, "MEMBUF");
      memcpy(remote_credentials, membuf + 0x08, 0x100);
      show_memory(remote_credentials, 0x100, "remote credentials");
    } else {
      debuglog("Remote Credentials blob not found\n");
      return -1;
    };



    //decrypt/unsign credentials by skype_pub
    _get_unsign_cred(remote_credentials, 0x100, tmpbuf);
    show_memory(tmpbuf, 0x100, "decrypt credentials");

    for (kk = 0; kk < (0x100 - 1); kk++) {
      if ((tmpbuf[kk] == 0x80) && (tmpbuf[kk + 1] == 0x01)) {
        debuglog("1 kk=0x%08X\n", kk);
        break;
      };
    };

    kk = kk + 2;
    debuglog("2 kk=0x%08X\n", kk);

    if ((kk + 0x80) < 0x100) {
      memcpy(remote_pubkey, tmpbuf + kk, 0x80);
    } else {
      debuglog("failed to find pubkey in credentials, kk=0x%08X\n", kk);
      return -1;
    };

    show_memory(remote_pubkey, 0x80, "remote peer pubkey");

    //remote_credentials, 0x100
    //remote_pubkey, 0x80
    add_cert(remote_credentials, remote_pubkey);

    // for new 4-11 blob creation
    //memcpy(REMOTE_AUTHORIZED188, membuf, 0x108);

    debuglog("Forming 04-11 'buddyauthorized' based on Remote Credentials\n");
    //buddy_authorize_cert2();

  };



  /////////////////////
  // SHA1 digest
  /////////////////////
  //make hash of remote rnd64bit challenge(8byte) + 0x01(9byte)
  if (1) {
    char *buf;
    char *outbuf;

    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);

    //prepare data for hashing
    memset(buf, 0x1, 0x9);
    memcpy(buf, rnd64bit, 8);

    //print it
    show_memory(buf, 9, "SHA1 input");

    //make sha1 hash
    //get_sha1_data(buf, 9, outbuf);
    _get_sha1_data(buf, 9, outbuf, 1);


    //print it
    show_memory(outbuf, 0x14, "SHA1 output(hash)");

    //copy hash
    memcpy(sha1, outbuf, 0x14);

  };



  ///////////////////////
  //RSA sign
  ///////////////////////
  //for sign rnd64bit challenge and sha1 hash of it
  if (1) {
    char *buf;
    char *outbuf;

// response on challenge
    u8 challenge[] =
        "\x4B\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB"
            "\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB"
            "\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB"
            "\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB"
            "\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB"
            "\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB\xBB"
            "\xBB\xBA\x66\xCE\x3F\xDB\xAA\x55\xB4\xF7\x01\xE9\x26\x8E\x38\x4C"
            "\x3C\x06\x30\xF8\xD9\xA4\xBF\x47\x63\xDC\xB8\x4C\x33\xCF\x2C\xBC";
//padding
//64bit challenge
//sha160bit hash


    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);


    //copy challenge template
    memcpy(buf, challenge, 0x80);

    //modify sha1 hash in challenge response
    memcpy(buf + 0x80 - 0x14 - 1, sha1, 0x14);

    //modify rnd64bit challenge in challenge response
    memcpy(buf + 0x62, rnd64bit, 8);

    //print challenge response data
    //before RSA sign-ing
    show_memory(buf, 0x80, "RSA SIGN input");

    //make rsa sign
    _get_sign_data(buf, 0x80, outbuf);

    //copy rsa sign to challenge_response buffer
    //for send this response in next pkt
    memcpy(CHALLENGE_RESPONSE, outbuf, 0x80);

    //print rsa signed challenge response data
    show_memory(CHALLENGE_RESPONSE, 0x80, "RSA SIGN output");

  };


  return 0;
};


int on_recv_BLOB_4_6(char *membuf, int membuf_len) {
  char nonce[0x80];



  // copy encrypted nonce from 41 encoding blob
  memcpy(nonce, membuf, membuf_len);

  // display crypted nonce
  show_memory(nonce, 0x80, "RSA encrypted remote nonce");


  /////////////////////////////
  // RSA decode
  /////////////////////////////
  // for decrypting remote nonce
  if (1) {
    char *buf;
    char *outbuf;

    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);

    // copy encrypted nonce
    memcpy(buf, nonce, 0x80);

    // rsa decrypt nonce
    show_memory(buf, 0x80, "Before RSA decrypt nonce");
    _get_decode_data(buf, 0x80, outbuf);
    show_memory(outbuf, 0x80, "After RSA decrypt nonce");

    // copy decrypted nonce
    memcpy(nonce, outbuf, 0x80);

  };



  ///////////////////////
  // pre-defined data
  ///////////////////////

  // aes key nonce1 (local)

  //for old xot_iam key
  //memcpy(aes_key,"\xA9\x45\x5C\x42\x7E\xCC\x79\x52\xF8\xA3\x07\xBD\xEA\xC8\x5B\x35",0x10);
  //memcpy(aes_key,"\xBD\x2E\xC3\x04\x10\xD8\x29\x03\x1A\xE4\x00\x97\x94\xB2\x3B\xE4",0x10);

  //for xot_iam
  //memcpy(aes_key,"\xE5\x9A\xA2\x55\xFD\xFF\xE5\xA0\x13\x66\xC8\x15\x3C\x69\x6D\xE6",0x10);

  //for xotabba
  //memcpy(aes_key,"\xC5\xC9\xEA\x82\x77\xFC\x51\x3C\x1A\xB2\xF1\x37\xEE\xCF\x4B\x39",0x10);




  /////////////////////////////
  // SHA1 digest
  /////////////////////////////
  // for getting aes key nonce2 (remote)
  if (1) {
    char *buf;
    char *outbuf;

    buf = malloc(0x1000);
    outbuf = malloc(0x1000);

    memset(buf, 0, 0x200);
    memset(outbuf, 0, 0x200);

    // first integer is 0x00000000 !!!
    memcpy(buf + 4, nonce, 0x80);

    // show data for hashing
    // integer(0x00) and 0x80 nonce
    show_memory(buf, 0x84, "NONCE input");

    // making sha1 hash on nonce
    //get_sha1_data(buf, 0x84, outbuf, 1);

    _get_sha1_data(buf, 0x84, outbuf, 0);


    // copy remote part of aes key
    memcpy(aes_key + 0x10, outbuf, 0x10);

    // show full aes session key
    show_memory(aes_key, 0x20, "AES KEY");

  };

  return 0;
};


// get 05-2F 00-02: blob
int get_00_02_blob_new(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-02 blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x02);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x02);
    debuglog("00-02: 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-02 in 6D --> 05-03 --> 04-04 --> 05-2F\n");
    //return 1;
  };

  return data_int;
};


unsigned int get_00_07_blob(char *membuf, int membuf_len) {
  unsigned int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-07 (unknown_cmd24_signed_id) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x07);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x07);
    debuglog("00-07: 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-07 in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_00_09_blob2(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-09 (unknown_cmd24_time) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x09);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x09);
    debuglog("00-09: 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-09 in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_00_1B_blob(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-1B (unknown_cmd24_blob1b) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x1B);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x1B);
    debuglog("00-1B: 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-1B in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_00_0A_blob(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-0A (remote header_id) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x0A);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x0A);
    debuglog("00-0A (remote HEADER_ID): 0x%08X\n", data_int);
    // maybe need change some global chat control state? or stage?
    //HEADER_ID = data_int;
    //debuglog("HEADER_ID: 0x%08X\n", HEADER_ID);
  } else {
    debuglog("not found blob 00-0A in 6D --> 05-03 --> 04-04\n");
    // blob not found
    // this is not "HEADER_ID" 6D packet...
    //return 1;
  };

  return data_int;
};


int get_00_0A_blob_last(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-0A (remote header_id) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x0A);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x0A);
    debuglog("00-0A (remote HEADER_ID): 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-0A in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_00_15_blob_last(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-15 (remote header_id_crc) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x15);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x15);
    debuglog("00-15 (remote HEADER_ID_CRC): 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-15 in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_00_0A_blob_first(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-0A (remote header_id) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x0A);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00_first(membuf, membuf_len, &data_int, 0x00, 0x0A);
    debuglog("00-0A (remote HEADER_ID): 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-0A in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_00_15_blob_first(char *membuf, int membuf_len) {
  int data_int;
  int ret;

  data_int = 0;
  debuglog("Looking for 00-15 (remote header_id_crc) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x15);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    main_unpack_getobj00_first(membuf, membuf_len, &data_int, 0x00, 0x15);
    debuglog("00-15 (remote HEADER_ID_CRC): 0x%08X\n", data_int);
  } else {
    debuglog("not found blob 00-15 in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return data_int;
};


int get_01_36_blob(char *membuf, int membuf_len, u8 *rnd64bit) {
  u8 data_64bit[0x08];
  int ret;
  int data_int;

  data_int = 0;
  debuglog("Looking for 01-36 (remote header_id) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x01, 0x36);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    memset(data_64bit, 0, 0x08);
    main_unpack_getobj01(membuf, membuf_len, data_64bit, 0x01, 0x36);
    memcpy(rnd64bit, data_64bit + 4, 4);
    memcpy(rnd64bit + 4, data_64bit, 4);
    show_memory(rnd64bit, 8, "01-36 (remote HEADER_ID_CRC)");
    data_int = 1;
  } else {
    debuglog("not found blob 01-36 in 6D --> 05-03 --> 04-04\n");
    return -1;
  };

  return data_int;
};


int get_03_02_blob(char *membuf, int membuf_len, char *output) {
  int ret;
  u8 remote_chatstring[0x100];
  int remote_chatstring_len;

  remote_chatstring_len = 0;

  debuglog("Looking for 03-02 (remote chatstring) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x03, 0x02);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    ret = main_unpack_getbuf(membuf, membuf_len, remote_chatstring, &remote_chatstring_len, 0x03, 0x02);
    debuglog("remote_chatstring: %s\n", remote_chatstring);
    debuglog("remote_chatstring_len: %d bytes\n", remote_chatstring_len);
    memcpy(output, remote_chatstring, remote_chatstring_len);
  } else {
    debuglog("Not found blob 03-02 in 6D --> 05-03 --> 04-04\n");
    //return 1;
  };

  return remote_chatstring_len;
};


//
// 04-03 msg count calc
//
int get_04_03_blob_count(char *membuf, int membuf_len) {
  int ret;
  int blobs_count;

  blobs_count = 0;

  debuglog("Looking for 04-03 (remote msg buf) blob...\n");
  ret = main_unpack_checkblob(membuf, membuf_len, 0x04, 0x03);
  if (ret == 1) {
    debuglog("BLOB found!\n");
    ret = main_unpack_getbuf_count(membuf, membuf_len, &blobs_count, 0x04, 0x03);
    debuglog("blobs_count_total: %d \n", blobs_count);
  } else {
    debuglog("Not found blob 04-03 in 6D --> 05-03 --> 04-04 --> 05-20\n");
  };

  return blobs_count;
};


//
// 04-03 msg buf processing
//
//
// 04-03 signed block
//
int get_04_03_blob_one(u8 *buf, int len, u8 *membuf, int *membuf_len, int index) {
  int ret;

  ret = main_unpack_getbuf_one(buf, len, membuf, membuf_len, index, 0x04, 0x03);
  if (ret == 1) {
    if (membuf_len <= 0) {
      debuglog("unpack_getbuf size error\n");
      return -1;
    };
    debuglog("MEMBUF_LEN: %d bytes\n", *membuf_len);
    show_memory(membuf, *membuf_len, "MEMBUF");
  } else {
    debuglog("04-03 blob not found\n");
    return -1;
  };

  return 1;
};


//
//
//
int get_headers_chain_blobs_seq(char *membuf, int membuf_len) {
  int data_int;
  int ret;
  int index;


  debuglog("00-09 blobs:\n");

  // for 00-09 (remote_headers_id)
  data_int = 0;
  index = 0;
  do {
    debuglog("index = %d\n", index);
    ret = main_unpack_getobj00_seq(membuf, membuf_len, &data_int, index, 0x00, 0x09);
    if (ret) {
      debuglog("data_int = %08X\n", _bswap32(data_int));
      //debuglog("ret = %d\n", ret);
      add_header_chain(data_int, 0x00, 0x00, index);
      index++;
    };
  } while (ret);


  debuglog("00-0A blobs:\n");

  // for 00-0A (local_headers_id)
  data_int = 0;
  index = 0;
  do {
    debuglog("index = %d\n", index);
    ret = main_unpack_getobj00_seq(membuf, membuf_len, &data_int, index, 0x00, 0x0A);
    if (ret) {
      debuglog("data_int = %08X\n", _bswap32(data_int));
      //debuglog("ret = %d\n", ret);
      add_header_chain(0x00, data_int, 0x00, index);
      index++;
    };
  } while (ret);


  debuglog("00-15 blobs:\n");

  // for 00-15 (header_id_crc)
  data_int = 0;
  index = 0;
  do {
    debuglog("index = %d\n", index);
    ret = main_unpack_getobj00_seq(membuf, membuf_len, &data_int, index, 0x00, 0x15);
    if (ret) {
      debuglog("data_int = %08X\n", _bswap32(data_int));
      //debuglog("ret = %d\n", ret);
      add_header_chain(0x00, 0x00, data_int, index);
      index++;
    };
  } while (ret);


  return 0;
};


int on_recv_A6_6D(char *pktbuf, int pktlen) {
  u8 sha1[0x14];
  u8 rnd64bit[0x8];


  /////////////////////////////////
  // Process received 6D pkt
  /////////////////////////////////

  show_memory(pktbuf, 5, "Header");


  /////////////////////////////
  // 41 decode
  // for getting remote HEADER_ID
  /////////////////////////////
  if (1) {
    u8 membuf[0x1000];
    int membuf_len;

    u8 membuf2[0x1000];
    int membuf2_len;

    int ret;
    int data_int;
    int data_int2;
    unsigned int remote_start_header;
    unsigned int count_cmd15;
    unsigned int remote_streamid;
    unsigned int size_00_06;


    debuglog("Looking for 05-03: { 00-01: XX XX XX XX } (remote chatsync streamid) blob...\n");
    ret = main_unpack_checkblob(pktbuf, pktlen, 0x00, 0x01);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      main_unpack_getobj00_last(pktbuf, pktlen, &remote_streamid, 0x00, 0x01);
      debuglog("Found 00-01 (remote chatsync streamid) blob  in 6D --> 05-03 \n");
      debuglog("It is 00-01: 0x%08X\n", remote_streamid);
      set_remote_chatsync_streamid(remote_streamid);
    };

    debuglog("Looking for 00-06 (size of full 04-04 if many msg) blob...\n");
    ret = main_unpack_checkblob(pktbuf, pktlen, 0x00, 0x06);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      main_unpack_getobj00_last(pktbuf, pktlen, &size_00_06, 0x00, 0x06);
      debuglog("Found 00-06 (04-04 not full pkt) blob  in 6D --> 05-03 \n");
      debuglog("It is 00-06: 0x%08X\n", size_00_06);

      RECV_MANYMSG_00_06_size_total = size_00_06;
      RECV_MANYMSG_00_06_flag = 1;
    };


    debuglog("Looking for 04-04 blob...\n");
    ret = main_unpack_checkblob(pktbuf, pktlen, 0x04, 0x04);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      main_unpack_getbuf(pktbuf, pktlen, membuf, &membuf_len, 0x04, 0x04);
      if (membuf_len <= 0) {
        debuglog("unpack_getbuf size error\n");
        return -1;
      };
      debuglog("MEMBUF_LEN: %d bytes\n", membuf_len);
      show_memory(membuf, membuf_len, "MEMBUF");

      // if we got 00-06 blob, process it
      if (RECV_MANYMSG_00_06_flag == 0) {
        main_unpack(membuf, membuf_len);
      } else {
        memcpy(RECV_MANYMSG_BUF + RECV_MANYMSG_BUF_LEN, membuf, membuf_len);
        RECV_MANYMSG_BUF_LEN += membuf_len;
        RECV_MANYMSG_00_06_size_recv += membuf_len;
      };

      // if we got full buffer

      if (RECV_MANYMSG_00_06_size_total > 0) {
        if (RECV_MANYMSG_00_06_size_recv == RECV_MANYMSG_00_06_size_total) {
          show_memory(RECV_MANYMSG_BUF, RECV_MANYMSG_BUF_LEN, "RECV_MANYMSG_BUF");
          main_unpack(RECV_MANYMSG_BUF, RECV_MANYMSG_BUF_LEN);
          RECV_MANYMSG_00_06_flag = 0;
          RECV_MANYMSG_00_06_size_total = 0;

          recovery_signed_data3(RECV_MANYMSG_BUF, RECV_MANYMSG_BUF_LEN);
        };
      };

    } else {
      debuglog("04-04 blob not found\n");
      // blob not found
      return 1;
    };


    // now processing data in 04-04 blob


    debuglog("Looking for 00-01 (CMD) blob...\n");
    ret = main_unpack_checkblob(membuf, membuf_len, 0x00, 0x01);
    if (ret == 1) {
      debuglog("BLOB found!\n");
      main_unpack_getobj00(membuf, membuf_len, &data_int, 0x00, 0x01);
      debuglog("Found blob 00-01 in 6D --> 05-03 --> 04-04\n");
      debuglog("It is 00-01: 0x%08X\n", data_int);

      // logging A6 01 packets separately
      ret = do_pktlog_cmd(data_int);
      if (ret < 0) {
        return ret;
      };

      RECV_CHAT_COMMANDS[RECV_CHAT_COMMANDS_LEN] = data_int;
      RECV_CHAT_COMMANDS_LEN++;

      if (data_int == 0x0D) {
        u8 remote_str[0x100];
        int remote_str_len;
        // extract remote chat_string and save it
        debuglog("::::::::::::::Got remote CHAT_STRING pkt\n");
        remote_str_len = get_03_02_blob(membuf, membuf_len, remote_str);
        if (remote_str_len > 0) {
          make_tcp_client_set_chatinit(remote_str, remote_str_len);
        };
        set_remote_cmd0D_streamid(remote_streamid);
      };

      if (data_int == 0x0F) {
        // prodoljaem chat_string session
        // zapuskaem SYNC-er, no sprashivaem s kakogo-to mesta? to est zapros zagolovkov
        debuglog("::::::::::::::Session CHAT_STRING OK!\n");
      };

      if (data_int == 0x10) {
        // moi posslednie sinhronizirovannie soobsheniya na etom header_id ostanovilis
        // message_at == until_mid = 29248270
        //onIAmSyncingHere

        data_int2 = get_00_0A_blob(membuf, membuf_len);

        debuglog("::::::::::::::Session CHAT_STRING HEADER_ID OK!\n");
        debuglog("::::::::::::::Got onIAmSyncingHere message_at 0x%08X\n", HEADER_ID_REMOTE_LAST);
        debuglog("::::::::::::::(remote part LAST KNOWN header)\n");

        if (data_int2 == 0xFFFFFFFF) {
          NO_HEADERS_FLAG = 1;
        } else {
          //HEADER_ID_REMOTE_LAST = data_int2;
          //HEADER_ID_REMOTE_LAST = data_int2;

          //HEADER_ID_CMD13R3_ID = data_int2;
          HEADER_ID_CMD13R4_ID = data_int2;

          // needed only on init session for some cases?
          if (newchatinit_flag) {
            HEADER_ID_LOCAL_FIRST = data_int2;
            HEADER_ID_LOCAL_LAST = data_int2;
          };

        };

      };

      if (data_int == 0x13) {

        // for some cases when first chatinit not really first at today (after clear profile)
        remote_start_header = get_00_0F_blob(membuf, membuf_len);
        debuglog("00-0F (remote_start_header) 0x%08X\n", remote_start_header);

        data_int2 = get_00_02_blob_new(membuf, membuf_len);
        if (data_int2 != 0) {
          GOT_REMOTE_MSG_COUNT = data_int2;
          // need because of first init headers contain two steps msgs
          if (newchatinit_flag) {
            GOT_REMOTE_MSG_COUNT--;
          };
          debuglog("GOT_REMOTE_MSG_COUNT = %d\n", GOT_REMOTE_MSG_COUNT);
        };

        if (data_int2 != 0) {

          get_headers_chain_blobs_seq(membuf, membuf_len);
          dump_headers();

          debuglog("newchatinit_flag: %d\n", newchatinit_flag);

          // a bit different cmd13recv processing for newchat session and restore session
          if (newchatinit_flag) {

            //update_headers_in_db();

            count_cmd15 = count_cmd15_headers(remote_start_header);
            if (GOT_REMOTE_MSG_COUNT != count_cmd15) {
              GOT_REMOTE_MSG_COUNT = count_cmd15;
            };
            debuglog("GOT_REMOTE_MSG_COUNT = %d\n", GOT_REMOTE_MSG_COUNT);

          } else {

            // HEADER_ID_CMD27_ID -- as last good recv header_id
            count_cmd15 = count_cmd15_headers2(remote_start_header,
                                               HEADER_ID_CMD27_ID,
                                               &HEADER_ID_REMOTE_FIRST,
                                               &HEADER_ID_REMOTE_LAST);
            GOT_REMOTE_MSG_COUNT = count_cmd15;
            debuglog("GOT_REMOTE_MSG_COUNT = %d\n", GOT_REMOTE_MSG_COUNT);

          };


          // update db with remote_header_id for msg2
          if (newchatinit_flag) {
            uint local_header_id;
            uint remote_header_id;
            uint header_id_crc;

            debuglog("update db with remote_header_id for msg2\n");

            remote_header_id = 0;
            local_header_id = 0;
            header_id_crc = 0;

            // it must be always one chain and the first
            // hm, local and remote swap places
            get_header_chain(0, &local_header_id, &remote_header_id, &header_id_crc);

            // local_header_id -- for select
            // header_id_crc -- for select
            // remote_header_id -- for update
            update_remoteheader_in_db(local_header_id, header_id_crc, remote_header_id);

          };
          // end of update db


          if (newchatinit_flag) {
            debuglog("\nFirst slot:\n");
            data_int2 = get_00_0A_blob_first(membuf, membuf_len);
            if (data_int2 != 0) {
              HEADER_ID_REMOTE_FIRST = data_int2;
            };
            data_int2 = get_00_15_blob_first(membuf, membuf_len);
            if (data_int2 != 0) {
              HEADER_SLOT_CRC1 = data_int2;
            };

            /*
            // special case
            if (HEADER_ID_REMOTE_FIRST != remote_start_header) {
                GOT_REMOTE_MSG_COUNT++;
            };
            */

            debuglog("\nLast slot:\n");
            data_int2 = get_00_0A_blob_last(membuf, membuf_len);
            if (data_int2 != 0) {
              // hm?
              HEADER_ID_SEND = data_int2;
              HEADER_ID_REMOTE_LAST = data_int2;
            };
            data_int2 = get_00_15_blob_last(membuf, membuf_len);
            if (data_int2 != 0) {
              // hm?
              HEADER_ID_SEND_CRC = data_int2;
              HEADER_SLOT_CRC2 = data_int2;
            };

          }; //end if newchatinit_flag

          debuglog("HEADER_ID_REMOTE_FIRST = %08X\n", HEADER_ID_REMOTE_FIRST);
          debuglog("HEADER_ID_REMOTE_LAST = %08X\n", HEADER_ID_REMOTE_LAST);
        };

        debuglog("::::::::::::::Session REMOTE SENDING HEADERS OK!\n");
        debuglog("::::::::::::::Got onIAmSyncingHere message_at 0x%08X\n", HEADER_ID_REMOTE_LAST);
        debuglog("::::::::::::::(remote part LAST KNOWN header)\n");
        debuglog("::::::::::::::GOOD!!!\n");
        debuglog("::::::::::::::Got HEADER_ID_SEND_CRC: 0x%08X\n", HEADER_ID_SEND_CRC);
      };

      if (data_int == 0x15) {
        // ok, poshli mne msg s etim header_id, ya vibral
        data_int2 = get_00_0A_blob(membuf, membuf_len);
        // header_id to send in msg pkt
        HEADER_ID_SEND = data_int2;
        debuglog("::::::::::::::Session CHAT_STRING HEADER_ID_SEND OK!\n");
        debuglog("::::::::::::::Got onIAmSyncingHere message_at 0x%08X\n", HEADER_ID_SEND);
        debuglog("::::::::::::::(remote part READY header)\n");
        if (HEADER_ID_SEND != (HEADER_ID_REMOTE_LAST + 1)) {
          // some stange error... we got not our headers reply...
          debuglog("hmm, headers not match... (HEADER_ID_SEND != HEADER_ID_REMOTE_LAST+1)\n");
          //return -1;
        };
        debuglog("HEADER_ID_SEND: 0x%08X\n", HEADER_ID_SEND);
      };

      // 0xCF0522DE -- our local seq session id
      //if (data_int == 0xCF0522DE) {
      //    // and 00-00: 01 -- error packet received, chat mode EMPTY
      //    debuglog("::::::::::::::Error or session empty packet received\n");
      //};

      if (data_int == 0x1D) {
        debuglog("::::::::::::::Session uic request received (or possible some error occurred)\n");
      };

      if (data_int == 0x23) {
        // zapuskaem SYNC-er s samogo nachala?
        NEWSESSION_FLAG = 1;
        debuglog("::::::::::::::Session CHAT_STRING unknown\n");
        debuglog("::::::::::::::Needed packet with CHAT_STRING signed\n");
        debuglog("::::::::::::::Needed init CHAT_STRING session again\n");

        debuglog("::::::::::::::(recv mode) Need re-establish chatsession\n");
      };

      if (data_int == 0x24) {
        debuglog("::::::::::::::Session CHAT_STRING SIGNED (newblk1) pkt recv\n");

        // function needs data of whole pkt
        recovery_signed_data(pktbuf, pktlen);
        data_int2 = get_00_07_blob(membuf, membuf_len);
        if (data_int2 != 0) {
          global_unknown_cmd24_signed_id = data_int2;
          debuglog("global_unknown_cmd24_signed_id = 0x%08X\n", global_unknown_cmd24_signed_id);
        }

        data_int2 = get_00_09_blob2(membuf, membuf_len);
        if (data_int2 != 0) {
          global_unknown_cmd24_time = data_int2;
          debuglog("global_unknown_cmd24_time = 0x%08X\n", global_unknown_cmd24_time);
        }

        data_int2 = get_00_1B_blob(membuf, membuf_len);
        if (data_int2 != 0) {
          global_unknown_cmd24_blob1b = data_int2;
          debuglog("global_unknown_cmd24_blob1b = 0x%08X\n", global_unknown_cmd24_blob1b);
        }

      };

      if (data_int == 0x27) {
        u8 rnd64bit[0x8];//FIXME: hides previous declaration

        debuglog("::::::::::::::Session SYNC FINISH HEADER_ID OK!\n");

        data_int2 = get_01_36_blob(membuf, membuf_len, rnd64bit);

        // only need this on chatinit
        //if ((data_int2) && (newchatinit_flag)) {
        if (data_int2) {
          memcpy(&HEADER_ID_CMD27_ID, rnd64bit, 4);
          memcpy(&HEADER_ID_CMD27_CRC, rnd64bit + 4, 4);
        };

      };

      if (data_int == 0x29) {
        debuglog("::::::::::::::Session chatsign accepted or request headers sign\n");
      };

      if (data_int == 0x2A) {
        debuglog("::::::::::::::Session HEADERS SIGNED (newblk2) pkt recv\n");
        // functions need data of whole pkt
        recovery_signed_data(pktbuf, pktlen);
      };

      if (data_int == 0x2B) {

        debuglog("::::::::::::::Session MSG BODY RECEIVED!\n");

        // if msgs not big, decode now, or decode later when all packets arrive
        if (RECV_MANYMSG_00_06_flag == 0) {
          // Now need decode msg, check signature and print it to user
          recovery_signed_data2(pktbuf, pktlen);
          do_proto_log_cryptodecode(pktbuf, pktlen, "recv_signed_decode");
        };

      };

    } else {
      debuglog("not found blob 00-01 in 6D --> 05-03 --> 04-04\n");
      // blob not found
      //return 1;
    };

  };


  return 0;
};


//
// main recv loop
//
int process_recv_data(char *recvbuf, int recvlen) {
  int tmplen;
  int blkseq;

  u8 new_recvbuf[0x1000];
  int new_recvlen;

  int checked_pkt_len;
  int i;

  int packet_type;

  //
  // PKT'S Processing
  //

  //debuglog("Process AES pkts in 57 41\n");

  debuglog("\nProcess AES pkts with 41\n");

  // init from prev processing
  checked_pkt_len = 0;

  i = 0;
  // main loop
  while (checked_pkt_len < recvlen) {
    int header_len;
    int AES_DATA_len;
    u8 membuf[0x1000];
    int membuf_len;
    int ret;

    i++;

    debuglog("\n:: PROCESSING PKT %d ::\n", i);

    // show header
    show_memory(recvbuf + checked_pkt_len, 5, "Header");


    // pass 1-3 bytes of header
    header_len = 0;
    tmplen = get_packet_size2(recvbuf + checked_pkt_len, 4, &header_len);
    tmplen = tmplen - 1;

    //tmplen pkt_size?

    checked_pkt_len = checked_pkt_len + header_len;


    // pass marker byte of AES crypto
    debuglog("Marker byte: 0x%02X\n", recvbuf[checked_pkt_len]);
    if (recvbuf[checked_pkt_len] != 0x05) {

      //debuglog("%08X %08X\n", recvbuf[checked_pkt_len+2], recvbuf[checked_pkt_len+3]);

      // Not aes crypto, just pkt
      if (recvbuf[checked_pkt_len + 2] == 0x07) {
        int last_recv_pkt_num;

        not_aes_counter++;
        debuglog("Not_aes_counter: %d\n", not_aes_counter);

        if (not_aes_counter >= 2) {
          debuglog("Timeout occured during cmd wait, returning...\n");
          return -100;
        };

        debuglog("Marker byte: 0x07. Received not AES encoded packet.\n");

        main_unpack42(recvbuf + 6, tmplen - 6);

        memcpy(&last_recv_pkt_num, recvbuf + checked_pkt_len, 2);
        debuglog("last_recv_pkt_num: %08X\n", last_recv_pkt_num);
        skyrel_answer_ack(last_recv_pkt_num);

        checked_pkt_len = checked_pkt_len + tmplen + header_len;

        continue;

      } else {
        debuglog("Marker byte error, do return...\n");
        return -1;
      };
    };
    checked_pkt_len++;

    // reset not aes counter
    not_aes_counter = 0;

    // pass two bytes of AES pkt id
    debuglog("AES pkt id: 0x%02X%02X\n", recvbuf[checked_pkt_len], recvbuf[checked_pkt_len + 1]);
    checked_pkt_len = checked_pkt_len + 2;


    debuglog("pkt len: 0x%08X\n", tmplen);
    debuglog("checked_pkt_len: 0x%08X\n", checked_pkt_len);
    debuglog("fullpkt len (recvlen): 0x%08X\n", recvlen);

    // some untested fix
    if (checked_pkt_len >= recvlen) {
      debuglog("This is half of packet, need recv more\n\n");

      new_recvlen = tcp_talk_recv2(new_recvbuf);
      show_memory(new_recvbuf, new_recvlen, "next recv buf:");

      // remove rc4 layer
      show_memory(new_recvbuf, new_recvlen, "Before RC4 decrypt");
      RC4_crypt(new_recvbuf, new_recvlen, &rc4_recv, 0);
      show_memory(new_recvbuf, new_recvlen, "After RC4 decrypt");

      memcpy(recvbuf + recvlen, new_recvbuf, new_recvlen);
      recvlen = recvlen + new_recvlen;

      show_memory(recvbuf, recvlen, "all together:");
    };


    // some untested fix2 PKT SIZE (in header) > recvlen (really recv data)
    if (tmplen > recvlen) {
      debuglog("This is half of packet, need recv more\n\n");

      new_recvlen = tcp_talk_recv2(new_recvbuf);
      show_memory(new_recvbuf, new_recvlen, "next recv buf:");

      // remove rc4 layer
      show_memory(new_recvbuf, new_recvlen, "Before RC4 decrypt");
      RC4_crypt(new_recvbuf, new_recvlen, &rc4_recv, 0);
      show_memory(new_recvbuf, new_recvlen, "After RC4 decrypt");

      memcpy(recvbuf + recvlen, new_recvbuf, new_recvlen);
      recvlen = recvlen + new_recvlen;

      show_memory(recvbuf, recvlen, "all together:");
    };


    // aes len = -2 bytes from start, -2 bytes from end
    // -2 from end used as crc of whole pkt
    AES_DATA_len = tmplen - 4;
    blkseq = get_blkseq(recvbuf + checked_pkt_len, AES_DATA_len + 2);

    if (GLOBAL_STATE_MACHINE == AES_KEY_OK) {
      process_aes_crypt(recvbuf + checked_pkt_len, AES_DATA_len, 1, blkseq, 1);
      main_unpack(recvbuf + checked_pkt_len, AES_DATA_len);
      //do_proto_log(recvbuf+checked_pkt_len, AES_DATA_len, "recv");
    };

    if (GLOBAL_STATE_MACHINE == AES_KEY_INIT) {
      process_aes_crypt(recvbuf + checked_pkt_len, AES_DATA_len, 0, blkseq, 0);
      main_unpack(recvbuf + checked_pkt_len, AES_DATA_len);
      do_proto_log(recvbuf + checked_pkt_len, AES_DATA_len, "setup_recv");
    };


    // pass 1-3 bytes of AES PKT DATA ID header (second AES data header?)
    header_len = 0;
    get_packet_size3(recvbuf + checked_pkt_len, 4, &header_len);

    packet_type = (int) recvbuf[checked_pkt_len + header_len] & 0xFF;
    debuglog("PACKET TYPE: 0x%02X\n", packet_type);

    if (packet_type == 0x44) {
      on_recv_44_41(recvbuf + checked_pkt_len, AES_DATA_len);
    };

    if (packet_type == 0x57) {

      debuglog("Looking for 4-6 blob...\n");
      ret = main_unpack_checkblob(recvbuf + checked_pkt_len, AES_DATA_len, 0x04, 0x06);
      if (ret == 1) {
        debuglog("BLOB found!\n");
        main_unpack_getbuf(recvbuf + checked_pkt_len, AES_DATA_len, membuf, &membuf_len, 0x04, 0x06);
        debuglog("MEMBUF_LEN: %d bytes\n", membuf_len);
        show_memory(membuf, membuf_len, "MEMBUF");
        on_recv_BLOB_4_6(membuf, membuf_len);
        GLOBAL_STATE_MACHINE = AES_KEY_OK;
        debuglog("AES STREAM READY\n");
        //send_first_three_pkt();
      };

    };


    if (packet_type == 0xA6) {
      int data_A6_type;
      int data_A6_sessid;

      data_A6_type = 0;
      data_A6_sessid = 0;


      // get remote A6_pkt_type
      debuglog("Looking for 00-01 (A6 pkt type) blob...\n");
      ret = main_unpack_checkblob(recvbuf + checked_pkt_len, AES_DATA_len, 0x00, 0x01);
      if (ret == 1) {
        debuglog("BLOB found!\n");
        main_unpack_getobj00(recvbuf + checked_pkt_len, AES_DATA_len, &data_A6_type, 0x00, 0x01);
        debuglog("A6 pkt type: 0x%02X\n", data_A6_type);
      };


      // get remote A6_sess_id
      debuglog("Looking for 00-02 (A6 session id) blob...\n");
      ret = main_unpack_checkblob(recvbuf + checked_pkt_len, AES_DATA_len, 0x00, 0x02);
      if (ret == 1) {
        debuglog("BLOB found!\n");
        main_unpack_getobj00(recvbuf + checked_pkt_len, AES_DATA_len, &data_A6_sessid, 0x00, 0x02);
        debuglog("data_A6_sessid: 0x%04X\n", data_A6_sessid);
      };


      // logging A6 01 packets separately
      ret = do_pktlog_A6_type(data_A6_type);
      if (ret < 0) {
        return ret;
      };


      // just sending ack-s
      if (data_A6_type == 0x7D) {
        debuglog("Sending 0x7D ack\n");
        make_tcp_client_sess1_send_A6_ack(data_A6_sessid);
      };

      if (data_A6_type == 0x7A) {
        debuglog("Sending 0x7A ack\n");
        make_tcp_client_sess1_send_A6_ack(data_A6_sessid);
      };

      if (data_A6_type == 0x7B) {
        debuglog("Sending 0x7B ack\n");
        make_tcp_client_sess1_send_A6_ack(data_A6_sessid);
      };

      if (data_A6_type == 0x53) {
        debuglog("Got 0x53 credentials pkt\n");
        debuglog("Sending 0x53 ack\n");
        make_tcp_client_sess1_send_A6_ack(data_A6_sessid);
      };

      if (data_A6_type == 0x6D) {
        debuglog("Got 0x6D pkt\n");
        debuglog("Sending 0x6D ack\n");
        make_tcp_client_sess1_send_A6_ack(data_A6_sessid);
      };



      // process 0x6D data packets
      if (data_A6_type == 0x6D) {
        debuglog("Got 0x6D chat_string pkt\n");
        ret = on_recv_A6_6D(recvbuf + checked_pkt_len, AES_DATA_len);
        if (ret < 0) {
          return ret;
        };
      };

      if (data_A6_type != 0) {
        do_proto_log(recvbuf + checked_pkt_len, AES_DATA_len, "recv");
      };

    };

    checked_pkt_len = checked_pkt_len + AES_DATA_len;

    // pass last two bytes of AES CRC
    debuglog("AES CRC: 0x%02X%02X\n", recvbuf[checked_pkt_len], recvbuf[checked_pkt_len + 1]);
    checked_pkt_len = checked_pkt_len + 2;


    debuglog("checked_pkt_len: 0x%08X\n", checked_pkt_len);
    debuglog("fullpkt len: 0x%08X\n", recvlen);

  };

  return 1;
};
