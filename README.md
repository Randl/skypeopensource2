# skypeopensource2

First in world self-made client to Skype Network based on reversing original Skype 5.5 client.

For now, only receiving and sending messages are supported.

As for now it compiles with Visual Studio (can't confirm if still working after changes done by me. You probably can get working copy from original repository) and MinGW (this one doesn't work for sure) on Windows 

## Disclaimer

Due to disagreements with @skypeopensource after which I was banned by him, his (in my opinion) poor infrastructure decisions and rejection of this project, it is now independent fork (i.e. no PRs to main repo). Yet all the credit for the reverse engeneering goes to @skypeopensource

## Goals and TODO
* Make a project compile on Linux
    + mainly to rewrite `winsock.h`-specific code
* Make project work on MinGw and gcc on Linux 
    + rewrite inline assembly code in plain C
* Make code 64-bit compatible
* Rewrite reverse engineered code to human-readable one
* Create simple usable API
* Create CLI client
* Create cross-platform GUI client (Qt)
* Add tests
* Add documentation


This is **very ambitious** plan and would require many hours of pretty big team to finish. I'm not hoping to do that alone since I don't have enough time and experience for that and pretty sceptical about whether that will be ever done at all. I only listed things that I believe **should be done** to make this project useful to others. As for now I'm focusing on a first goal, but welcome any contribution from anyone, both for anything listed above or not. Feel free to contact me in any way you find convenient.
## License
LGPL 3.0


## Updated 2016-09-12 02:00

Program tested, protocol still working OK.

But due to heavy testing during news headlines, function Skypename to IP fail because of static ip. 

Need change it and recompile as mention before.

## Updated 2016-09-08 20:00

Program tested, protocol still working OK.

But due heavy testing during news headlines, function Skypename to IP fail because of static ip used.

To workaround this problem, uncomment new ip address (the "good one") in this [code](https://github.com/skypeopensource/skypeopensource2/blob/master/skysearch3/skysearch3/skysearch3.c#L327):

```
    // main
    ip=strdup("157.55.235.147");
    port=40030;

    // good one
    //ip=strdup("65.55.223.14");
    //port=40016;
```

And recompile it.

You can also use this [project](https://github.com/skypeopensource/tool_linuxbuild/tree/master/skysearch2_linux) to do it on linux. And use result strings as vcard to Vcard area in program GUI (but do fail resolv at first, its need to get public ip).
I just tested it, it work good.

## Updated 2016-09-06 15:00

Program still working OK.

Some additional info on usage:

If you got this error on black console:

```
UIC_CRC = 5E8EBAE5
pkt block crc32=00009C54
relay_connect_mode = 1
LOCAL_SESSION_ID_RELAY = 0x00002619
total=0x00004C32
last2b=0x0000D066
encode len: 0x0000044A
```

And this error in "Debug Data" area:

```
Message send failed. Ret code:-1
```

Screenshot: http://epycs.ru/pics/cred_err.png

Then try to remove **a_cred.txt** file, close program and **run again**. 
Its some bug with bugged credentials. Which happens from time to time.

## Issue Statements (added 2016-09-04)
The README should clearly state:
* whether or not this client is a complete **re-implementation**
* whether or not it includes **disassembled** code from the other Skype clients
* whether or not it is a **clean-room reverse-engineered** code, like Wine/ReactOS
* why the specified **license is valid** and legal to use for the project
* what are the **legal complications**, if any, to developing the project and to using it personally or commercially

## Reply on issues

> whether or not this client is a complete re-implementation

Yes. It's a complete re-implementation of proprietary Skype Network protocol.

> whether or not it includes disassembled code from the other Skype clients

Well, maybe a little :) ```unpack-4142.c``` and ```pack-4142.c``` are HexRays decompiled code and should be rewritten but I don't have time for this now.

> whether or not it is a clean-room reverse-engineered code, like Wine/ReactOS

No. Not clean room, not Chinese wall.

> why the specified license is valid and legal to use for the project

It's my own code, I write it, I am the author of most code and I hold all copyright rights.

Well, except some login pieces (```skyauth3/skype_login.c```) which are copyrighted by Sean O'Neil from [VEST Corporation](https://en.wikipedia.org/wiki/VEST) in 2009 and RC4 reversing and implementation (```skype_rc4.c/skype_rc4.h```) also by Sean O'Neil.

> what are the legal complications, if any, to developing the project and to using it personally or commercially

1. The ```pack-4142.c/unpack41-42.c``` mentioned above. 
2. Also, it contains MIRACL for RSA and big numbers multiply arithmetics. 

> MIRACL Cryptographic SDK: Multiprecision Integer and Rational Arithmetic Cryptographic Library is a C software library that is widely regarded by developers as the gold standard open source SDK for elliptic curve cryptography (ECC). http://miracl.com https://github.com/miracl/MIRACL

For commercial usage and license send me an e-mail to skypeopensource@gmail.com

## Modules

* **goodrecvrelay** -- recv msg running flow
* **goodsendrelay** -- send msg sequences flow
* **skyauth** -- login process code
* **skycontact** -- contact list load from the event-server
* **skyrelay** -- some buggy version for "skype version check" its like send/recv msg but with handshake init packets only.
* **skysearch** -- resolver for skypename to get parent-node of the user and his IPs.
* **sqldbread** -- SQLite stuff for chat history and local_header_id and remote_header_id of message.

## Links

Protocol brief overview: 
http://skype-open-source2.blogspot.ru/2016/06/skype-protocol-dumps.html

Release post: 
http://skype-open-source2.blogspot.ru/2016/09/first-in-world-skype-network-compatible.html

Binaries:
http://epycs.ru/files/EpycsMessenger2_20160901.zip
http://yadi.sk/d/xvKhmImdugThn