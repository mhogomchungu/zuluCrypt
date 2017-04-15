## zuluCrypt

zuluCrypt is currently ```Linux only``` and it does hard drives encryption and it can manage PLAIN dm-crypt volumes,LUKS encrypted volumes,TrueCrypt encrypted volumes and VeraCrypt encrypted volumes.

zuluCrypt can manage encrypted volumes that are hosted in image files,lvm,mdraid,hard drives,usb sticks or any other block device.

zuluCrypt can also encrypt stand alone files(zuluCrypt menu->zC->encrypt a file).

## zuluMount

zuluMount is bundled with zuluCrypt and its meant to be used as a general purpose tool that mount and unmount zuluCrypt supported encrypted volumes as well as unencrypted volumes and it can be used as a substitute to udisks,pmount and related tools.


zuluMount-gui can also be used as a frontend to <a href="https://wiki.archlinux.org/index.php/EncFS">encfs</a>,<a href="https://nuetzlich.net/gocryptfs">gocryptfs</a>,<a href="https://github.com/netheril96/securefs">securefs</a>,<a href="http://ecryptfs.org/">ecryptfs</a> and <a href="https://www.cryfs.org/">cryfs.</a>

zuluMount-gui is ideal for use as a desktop environment/file manager independent tool for mounting/unmounting encrypted and unencrypted volumes.

Some options of zuluMount-gui are set through its tray icon,if you are on a GNOME3 desktop,you can access those options through GNOME3 extension like <a href="https://extensions.gnome.org/extension/495/topicons/">topicons.</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/blob/master/docs/zuluCrypt.pdf">zuluCrypt.pdf</a> has more information about zuluCrypt and its supported encrypted volumes.


```My public key and contact info are at the end of this page.```


```Paypal account: currently none."mhogomchungu@gmail.com" is owned by somebody else and i have so far failed to get it back.```

## screen shots

screenshots are at the end of this page.

## video reviews

<a href="https://www.youtube.com/watch?v=Tb39XAsnixo">click this link to see a youtube video showing some features of zuluCrypt</a>

## zuluCrypt download page

latest released version is 5.1.0 and it can be downloaded by clicking any one of the two links below.

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0.tar.bz2">zuluCrypt-5.1.0.tar.bz2</a>

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0.tar.xz">zuluCrypt-5.1.0.tar.xz</a>


## binary packages

```Fedora and opensuse``` users can install binary packages by following instructions on <a href="http://software.opensuse.org/download.html?project=home%3Amhogomchungu&package=zuluCrypt">this</a> link

zuluCrypt packages are in <a href="https://admin.fedoraproject.org/pkgdb/package/rpms/zulucrypt/">Fedora repositories</a> and can be installed through normal/standard package installation procedures in Fedora.

```Archlinux``` users can get the latest version of zuluCrypt from <a href="https://aur.archlinux.org/packages/zulucrypt/">here.</a>

```Archlinux``` users can get the development version of zuluCrypt from <a href="https://aur.archlinux.org/packages/zulucrypt-git/">here.</a>

```Binary packages for debian and ubuntu are below:```


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-debian-7-Wheezy.tar.xz">zuluCrypt-5.1.0-debian-7-Wheezy.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-debian-8-Jessie.tar.xz">zuluCrypt-5.1.0-debian-8-Jessie.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-14.04-Trusty_Tahr.tar.xz">zuluCrypt-5.1.0-ubuntu-14.04-Trusty_Tahr.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-14.10-Utopic_Unicorn.tar.xz">zuluCrypt-5.1.0-ubuntu-14.10-Utopic_Unicorn.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-15.04-Vivid_Vervet.tar.xz">zuluCrypt-5.1.0-ubuntu-15.04-Vivid_Vervet.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-15.10-Wily_Werewolf.tar.xz">zuluCrypt-5.1.0-ubuntu-15.10-Wily_Werewolf.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-16.04-Xenial_Xerus.tar.xz">zuluCrypt-5.1.0-ubuntu-16.04-Xenial_Xerus.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-16.10-Yakkety_Yak.tar.xz">zuluCrypt-5.1.0-ubuntu-16.10-Yakkety_Yak.tar.xz</a>


<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/5.1.0/zuluCrypt-5.1.0-ubuntu-17.04-Zesty_Zapus.tar.xz">zuluCrypt-5.1.0-ubuntu-17.04-Zesty_Zapus.tar.xz</a>

These archives contains binaries for both 32 bit and 64 bit systems.


Archive signatures can be downloaded from <a href="https://github.com/mhogomchungu/zuluCrypt/releases/tag/5.1.0">here.</a>

Development versions of binary packages together with other generated package files can be obtained from <a href="http://download.opensuse.org/repositories/home:/mhogomchungu/">here.</a>

Older versions can be downloaded from <a href="https://github.com/mhogomchungu/zuluCrypt/releases">here.</a>


## cryptsetup

<a href="https://gitlab.com/cryptsetup/cryptsetup/blob/master/README.md">cryptsetup</a> is one of the most popular linux native solutions for block device encryption.

## tcplay

<a href="https://github.com/bwalex/tc-play">tcplay</a> is an independent,BSD licensed full implementation of truecrypt formatted encrypted volumes.


## VeraCrypt

<a href="https://veracrypt.codeplex.com/">VeraCrypt</a> is a security enhanced fork of TrueCrypt.

## key sources.

zuluCrypt has a plugin architecture that allows it to retrieve keys to unlock volume from difference source:

* gnome's libsecret(gnome wallet).

* kde's kwallet.

* an internal storage system powered by libgcrypt.

* a keyfile where key = contents of a keyfile.

* a combination of a passphrase and keyfile where key = passphrase + content of keyfile.

* a combination of a passphrase and keyfile where key = gpg_symmetric_decrypt(passphrase,keyfile)

* a combination of a passphrase and keyfile where key = hmac-sha256(passphrase,keyfile)

## where did the name come from?

The "crypt" part is there to show core feature of the tool. The "zulu" part come from "zulu time", commonly known as "GMT"( Greenwich Mean Time ). The tv was on as i was thinking of what name to use for the project and somebody on tv kept mentioning "zulu time"( i think it was a military related show ) and the zulu part of the name got stuck.

## various components of the tools

zuluCrypt is made up of two components: The back end,CLI interface is called "zuluCrypt-cli".it is build using C and has required dependencies on libblkid,libcryptsetup and an optional dependency on libtcplay. The front end is Qt GUI application called "zuluCrypt-gui",it is build using C++/Qt

zuluMount is made up of two components: The back end,CLI interface is called "zuluMount-cli".It is build using C and has a dependency on zuluCrypt-cli libraries. The front end is a Qt GUI application called "zuluMount-gui".It is build using C++/Qt

The GUI components are optional and the project can be build without them.They have a run-time optional dependency on <a href="http://linux.die.net/man/1/xdg-open">"xdg-open"</a>.If present and working properly,a default file manager will open a mount point folder automatically when a volume is opened/mounted.

## frequently asked questions
The list of most frequently asked questions can be found <a href="https://github.com/mhogomchungu/zuluCrypt/wiki/Frequently-Asked-Questions">here</a>.


## zuluCrypt screenshots

<a href="http://tinypic.com?ref=10n6hlc" target="_blank"><img src="http://i64.tinypic.com/10n6hlc.jpg" border="0" alt="Image and video hosting by TinyPic"></a>

<a href="http://tinypic.com?ref=1ak4i" target="_blank"><img src="http://i65.tinypic.com/1ak4i.png" border="0" alt="Image and video hosting by TinyPic"></a>

## zuluMount screenshot

<a href="http://tinypic.com?ref=im3t6f" target="_blank"><img src="http://i68.tinypic.com/im3t6f.png" border="0" alt="Image and video hosting by TinyPic"></a>

## Contact info:
```
Name: Francis Banyikwa
email: mhogomchungu@gmail.com

-----BEGIN PGP PUBLIC KEY BLOCK-----
Version: GnuPG v1

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
i/gJN6mX3AIU7HHEHnP5ty8mwbb8ROc=
=VDxL
-----END PGP PUBLIC KEY BLOCK-----
```

last update: Sat Apr 15 11:48:14 EAT 2017
