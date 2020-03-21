About
==========
tcplay is a free (BSD-licensed), pretty much fully featured (including multiple
keyfiles, cipher cascades, etc) and stable TrueCrypt implementation.

This implementation supports mapping (opening) both system and normal TrueCrypt
volumes, as well as opening hidden volumes and opening an outer volume while
protecting a hidden volume. There is also support to create volumes, including
hidden volumes, etc. Since version 1.1, there is also support for restoring
from the backup header (if present), change passphrase, keyfile and PBKDF2
PRF function.

Since tcplay uses dm-crypt (or dm_target_crypt on DragonFly) it makes full use
of any available hardware encryption/decryption support once the volume has been
mapped.

It is based solely on the documentation available on the TrueCrypt website,
many hours of trial and error and the output of the Linux' TrueCrypt client.
As it turns out, most technical documents on TrueCrypt contain mistakes, hence
the trial and error approach.


VeraCrypt support
==========
tcplay has support for creating, modifying and mapping VeraCrypt volumes. See
the man page for more details.


Implementation notes
==========
DragonFly BSD uses the hybrid OpenSSL + cryptodev(9) approach that can be
found in crypto-dev.c. OpenSSL is only used for the hash/pbkdf2. The
encryption/decryption is performed via cryptodev(9) with enabled cryptosoft.

On Linux gcrypt is used for the encryption and decryption. For the hash/pbkdf2
either gcrypt or OpenSSL can be used. gcrypt only supports pbkdf2 since its
July 2011 release (1.5.0), while OpenSSL has had pbkdf2 since around December
2010, so its easier to find in most distros.

The crypto options can be chosen with make/Makefile parameters. Building on Linux
is as easy as doing

    make -f Makefile.classic SYSTEM=linux

you can even skip the SYSTEM=linux, since that's the default. To choose the
PBKDF backend, you can use either,

    make -f Makefile.classic PBKDF_BACKEND=openssl

or

    make -f Makefile.classic PBKDF_BACKEND=gcrypt

The interface to device mapper is libdevmapper on Linux and libdm on DragonFly.
libdm is a BSD-licensed version of libdevmapper that I hacked together in a few
hours.

On Ubuntu, the following dev packages are needed to build tcplay:

    apt-get install build-essential libdevmapper-dev libgcrypt11-dev uuid-dev


cmake
----------
New in version 1.0 is a cmake build system. tcplay can now be built using:

    mkdir objdir
    cd objdir
    cmake ..
    make

NOTE: building inside the source directory is discouraged, so please do
build inside an "objdir" directory. If you happen to do it anyway, you can
clean up behind cmake using:

    make -f Makefile.classic clean_cmake_mess

Before running `cmake`, make sure you have `pkg-config` installed; e.g. on a
Debian or Ubuntu system you can install it using:

    apt-get install pkg-config

This process will check for dependencies and automatically select whether to
use OpenSSL or gcrypt as PBKDF backend.

In addition, this process will also generate a .pc file (pkg-config) for the
tcplay library.

The classic single-file Makefile can still be used for building, however, using

    make -f Makefile.classic

Or, if you only want the command line tool:

    make -f Makefile.classic tcplay


Library
==========
In addition to providing a command line tool, tcplay is also available as a
library. See the `tcplay.3` man page for more details on how to use the API.

TODO: link examples


Documentation
==========
Please refer to the man pages bundled with tcplay.



Download for packaging
==========
Latest release can be found as a (source) tarball at:

https://github.com/bwalex/tc-play/archive/v3.3.tar.gz



Bugs
==========
Please report all bugs on the github issue tracker. If appropriate, please
attach a small test volume which you think tcplay isn't handling correctly.
The reduce_test_vol.sh script in test/ can significantly reduce the size
of a volume when compressed by stripping out all the unnecessary data,
leaving only the headers. After that, just bzip2 it and it should be fairly
tiny.

What would be even better is if you could write a small test case to
reproduce the issue. The README in the test/ directory has information on
how to write tests for tcplay.



OS Support
==========
tcplay is now available for both DragonFly BSD and Linux. It is a core part of
the DragonFly BSD operating system and is available in a number of linux
distros.



Licensing
==========
The project is under a two-clause BSD license. I would consider dual-licensing
it if required. Drop me an email to discuss the options.



Development
==========
tcplay is pretty much stable, but if you find a bug, please report it.
If anyone wants to add new features or port it to another OS, I'll gladly merge
your changes into this repository so that there is a single point of contact.

I've noticed that sometimes bugs are only reported downstream (e.g. in the
distro's bugtracker). Please make sure those bugs are also reported upstream on
github, otherwise odds are they will never reach me.



Bugs in the TrueCrypt documentation
==========
The TrueCrypt documentation is pretty bad and does not really represent the
actual on-disk format nor the encryption/decryption process.

Some notable differences between actual implementation and documentation:
 - PBKDF using RIPEMD160 only uses 2000 iterations if the volume isn't a system
   volume.
 - The keyfile pool is not XOR'ed with the passphrase but modulo-256 summed.
 - Every field *except* the minimum version field of the volume header are in
   big endian.
 - Some volume header fields (creation time of volume and header) are missing
   in the documentation.
 - All two-way cipher cascades are the wrong way round in the documentation,
   but all three-way cipher cascades are correct.

