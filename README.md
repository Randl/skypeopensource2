# skypeopensource2

First in world self-made client to Skype Network based on reversing original Skype 5.5 client.

For now, only receiving and sending messages are supported.

Try to compile it with Visual Studio 2010 and test by youself or use already compiled binaries.

## License
LGPL 3.0

A commercial license is also avaiable on request.

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

## Links

Protocol brief overview: 
http://skype-open-source2.blogspot.ru/2016/06/skype-protocol-dumps.html

Release post: 
http://skype-open-source2.blogspot.ru/2016/09/first-in-world-skype-network-compatible.html

Binaries:
http://epycs.ru/files/EpycsMessenger2_20160901.zip
http://yadi.sk/d/xvKhmImdugThn

Sources (same as on GitHub here):
http://epycs.ru/files/EpycsMessenger2_20160901_src.zip
http://yadi.sk/d/V_IWJT-fugvND
