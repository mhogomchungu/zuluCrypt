## zuluCrypt

zuluCrypt is currently ```Linux only``` and it does hard drives encryption and it can manage PLAIN dm-crypt volumes, LUKS encrypted volumes, TrueCrypt encrypted volumes and VeraCrypt encrypted volumes.

zuluCrypt can manage encrypted volumes that are hosted in image files, lvm, mdraid, hard drives, usb sticks or any other block device.

zuluCrypt can also encrypt stand alone files (zuluCrypt menu -> zC -> encrypt a file).

## zuluMount

zuluMount is bundled with zuluCrypt and its meant to be used as a general purpose tool that mount and unmount zuluCrypt supported encrypted volumes as well as unencrypted volumes and it can be used as a substitute to udisks, pmount and related tools.


zuluMount-gui can also be used as a frontend to <a href="https://wiki.archlinux.org/index.php/EncFS">encfs</a>, <a href="https://nuetzlich.net/gocryptfs">gocryptfs</a>, <a href="https://github.com/netheril96/securefs">securefs</a>, <a href="http://ecryptfs.org/">ecryptfs</a> and <a href="https://www.cryfs.org/">cryfs</a>.

zuluMount-gui is ideal for use as a desktop environment/file manager independent tool for mounting/unmounting encrypted and unencrypted volumes.

Some options of zuluMount-gui are set through its tray icon, if you are on a GNOME3 desktop, you can access those options through GNOME3 extension like <a href="https://extensions.gnome.org/extension/495/topicons/">topicons</a>.

<a href="https://github.com/mhogomchungu/zuluCrypt/blob/master/docs/zuluCrypt.pdf">zuluCrypt.pdf</a> has more information about zuluCrypt and its supported encrypted volumes.


```My public key and contact info are at the end of this page.```

## DONATIONS

```
Paypal does not work where i live(United Republic of Tanzani)
and i accept donations only through Western Union and bank transfers.

1. Western Union information for donating through Western Union
Name:  Francis Nkulu Banyikwa
Email: banyikwafb@gmail.com

The email address should be used to send tracking number(MTCN)

2. Bank information for donating through bank tranfer:
Name:		Francis Nkulu Banyikwa
Bank Name:	CRDB BANK PLC
Bank Location:	Tegeta Kibo complex,
		Dar Es Salaam,
		United Republic of Tanzania
Swift Code:	CORUTZTZ
Account Number:	0152454497500
```

## Screen shots

Screenshots are at the end of this page.

## Video reviews

<a href="https://www.youtube.com/watch?v=Tb39XAsnixo">Click this link to see a youtube video showing some features of zuluCrypt.</a>

## zuluCrypt download page

Latest released version is 5.2.0 and it can be downloaded by clicking any one of the two links below.

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0.tar.bz2">zuluCrypt-5.2.0.tar.bz2</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0.tar.xz">zuluCrypt-5.2.0.tar.xz</a>

<script async src="//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js"></script>
<ins class="adsbygoogle"
     style="display:block; text-align:center;"
     data-ad-layout="in-article"
     data-ad-format="fluid"
     data-ad-client="ca-pub-8347348550354816"
     data-ad-slot="6481962469"></ins>
<script>
     (adsbygoogle = window.adsbygoogle || []).push({});
</script>

## Binary packages

Fedora and opensuse users can install binary packages by following instructions on <a href="http://software.opensuse.org/download.html?project=home%3Amhogomchungu&package=zuluCrypt">this link</a>.

Debian and Ubuntu users can install binary packages from a list given below.

zuluCrypt packages are also in Fedora,Debian and Ubuntu repositories and can be installed through normal/standard package installation procedures in those distributions.

```
zuluCrypt packages i provide are better than those provided by these distributions
because of the following reasons:

1. My packages do not generated a polkit prompt that requires a root password
   when GUI components are started.

2. My packages provides CLI components that work from a normal user account.
```


```Archlinux``` users can get the latest version of zuluCrypt from <a href="https://aur.archlinux.org/packages/zulucrypt/">here</a>.

```Archlinux``` users can get the development version of zuluCrypt from <a href="https://aur.archlinux.org/packages/zulucrypt-git/">here</a>.

```Binary packages for debian and ubuntu are below:```

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-debian-8-Jessie.tar.xz">zuluCrypt-5.2.0-debian-8-Jessie.tar.xz</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-debian-9-Stretch.tar.xz">zuluCrypt-5.2.0-debian-9-Stretch.tar.xz</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-ubuntu-14.04-Trusty_Tahr.tar.xz">zuluCrypt-5.2.0-ubuntu-14.04-Trusty_Tahr.tar.xz</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-ubuntu-16.04-Xenial_Xerus.tar.xz">zuluCrypt-5.2.0-ubuntu-16.04-Xenial_Xerus.tar.xz</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-ubuntu-16.10-Yakkety_Yak.tar.xz">zuluCrypt-5.2.0-ubuntu-16.10-Yakkety_Yak.tar.xz</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-ubuntu-17.04-Zesty_Zapus.tar.xz">zuluCrypt-5.2.0-ubuntu-17.04-Zesty_Zapus.tar.xz</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.2.0/zuluCrypt-5.2.0-ubuntu-17.10-Artful_Aardvark.tar.xz">zuluCrypt-5.2.0-ubuntu-17.10-Artful_Aardvark.tar.xzz</a>

These archives contains binaries for both 32 bit and 64 bit systems.


Archive signatures can be downloaded from <a href="https://github.com/mhogomchungu/zuluCrypt/releases/tag/5.2.0">here</a>.

Development versions of binary packages together with other generated package files can be obtained from <a href="http://download.opensuse.org/repositories/home:/mhogomchungu/">here</a>.

Older versions can be downloaded from <a href="https://github.com/mhogomchungu/zuluCrypt/releases">here</a>.


## cryptsetup

<a href="https://gitlab.com/cryptsetup/cryptsetup/blob/master/README.md">cryptsetup</a> is one of the most popular linux native solutions for block device encryption.

## zuluplay

<a href="https://github.com/mhogomchungu/zuluplay">zuluplay</a> is an independent, BSD licensed full implementation of truecrypt/veracrypt formatted encrypted volumes.


## VeraCrypt

<a href="https://www.veracrypt.fr/en/Home.html">VeraCrypt</a> is a security enhanced fork of TrueCrypt.

## key sources

zuluCrypt has a plugin architecture that allows it to retrieve keys to unlock volume from difference source:

* gnome's libsecret (gnome wallet)

* kde's kwallet

* an internal storage system powered by libgcrypt

* a keyfile where key = contents of a keyfile

* a combination of a passphrase and keyfile where key = passphrase + content of keyfile

* a combination of a passphrase and keyfile where key = gpg_symmetric_decrypt (passphrase, keyfile)

* a combination of a passphrase and keyfile where key = hmac-sha256 (passphrase, keyfile)

## Where did the name come from?

The "crypt" part is there to show core feature of the tool. The "zulu" part come from "zulu time", commonly known as "GMT" (Greenwich Mean Time). The tv was on as i was thinking of what name to use for the project and somebody on tv kept mentioning "zulu time" (i think it was a military related show) and the zulu part of the name got stuck.

## Various components of the tools

zuluCrypt is made up of two components: The back end, CLI is called "zuluCrypt-cli". It is build using C, has required dependencies on libblkid, libcryptsetup and an optional dependency on libtcplay. The front end is a Qt GUI application called "zuluCrypt-gui". It is build using C++/Qt.

zuluMount is made up of two components: The back end, CLI is called "zuluMount-cli". It is build using C and has a dependency on zuluCrypt-cli libraries. The front end is a Qt GUI application called "zuluMount-gui". It is build using C++/Qt.

The GUI components are optional and the project can be build without them. They have a run-time optional dependency on <a href="http://linux.die.net/man/1/xdg-open">"xdg-open"</a>. If present and working properly, a default file manager will open a mount point folder automatically when a volume is opened/mounted.

## Frequently asked questions
The list of most frequently asked questions can be found <a href="https://github.com/mhogomchungu/zuluCrypt/wiki/Frequently-Asked-Questions">here</a>.


## zuluCrypt screenshots

<a href="http://tinypic.com?ref=10n6hlc" target="_blank"><img src="http://i64.tinypic.com/10n6hlc.jpg" border="0" alt="Image and video hosting by TinyPic"></a>

<a href="http://tinypic.com?ref=1ak4i" target="_blank"><img src="http://i65.tinypic.com/1ak4i.png" border="0" alt="Image and video hosting by TinyPic"></a>

## zuluMount screenshot

<a href="http://tinypic.com?ref=im3t6f" target="_blank"><img src="http://i68.tinypic.com/im3t6f.png" border="0" alt="Image and video hosting by TinyPic"></a>

## Contact info:

There are two keys below. The top one is the one i currently
use and can use to decrypt ecrypted messages.
The bottom one i no longer use and have lost the ability to
decrypt messages encrypted with it but its used to
signed currently released packages. Version 5.3.0(unreleased)
will be signed with the new key.

```
Name: Francis Banyikwa
email: banyikwafb@gmail.com

gpg key finderpring: E3AF84691424AD00E099003502FC64E8DEBF43A8

Get the key with command: gpg --recv-keys 0x02FC64E8DEBF43A8

The key is below:

-----BEGIN PGP PUBLIC KEY BLOCK-----

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
9azeqex9
=L6yQ
-----END PGP PUBLIC KEY BLOCK-----

```

OLD CONTACT BELOW.
I still use the email address but i no longer have the ability
to decrypt messages encrypted with below key.

## Contact info:

```
-----BEGIN PGP PUBLIC KEY BLOCK-----
Version: GnuPG v1

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
i/gJN6mX3AIU7HHEHnP5ty8mwbb8ROc=
=VDxL
-----END PGP PUBLIC KEY BLOCK-----
```

last update: Wed Sep 27 08:47:00 EAT 2017
