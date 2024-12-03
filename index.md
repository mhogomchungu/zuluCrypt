## zuluCrypt

zuluCrypt is currently ```Linux only``` and it does hard drives encryption and it can manage PLAIN dm-crypt volumes, LUKS encrypted volumes, TrueCrypt encrypted volumes, VeraCrypt encrypted volumes and Microsoft's BitLocker volumes.

zuluCrypt can manage encrypted volumes that are hosted in image files, lvm, mdraid, hard drives, usb sticks or any other block device.

zuluCrypt can also encrypt stand alone files (zuluCrypt menu -> zC -> encrypt a file).

## zuluMount

zuluMount is bundled with zuluCrypt and its meant to be used as a general purpose tool that mount and unmount zuluCrypt supported encrypted volumes as well as unencrypted volumes and it can be used as a substitute to udisks, pmount and related tools.


zuluMount-gui can also be used as a frontend to <a href="https://wiki.archlinux.org/index.php/EncFS">encfs</a>, <a href="https://nuetzlich.net/gocryptfs">gocryptfs</a>, <a href="https://github.com/netheril96/securefs">securefs</a>, <a href="http://ecryptfs.org/">ecryptfs</a> and <a href="https://www.cryfs.org/">cryfs</a>.

zuluMount-gui is ideal for use as a desktop environment/file manager independent tool for mounting/unmounting encrypted and unencrypted volumes.

Some options of zuluMount-gui are set through its tray icon, if you are on a GNOME3 desktop, you can access those options through GNOME3 extension like <a href="https://extensions.gnome.org/extension/495/topicons/">topicons</a>.

<a href="https://github.com/mhogomchungu/zuluCrypt/blob/master/docs/zuluCrypt.pdf">zuluCrypt.pdf</a> has more information about zuluCrypt and its supported encrypted volumes.

## Screen shots

Screenshots are at the end of this page.

## Video reviews

<a href="https://www.youtube.com/watch?v=Tb39XAsnixo">Click this link to see a youtube video showing some features of zuluCrypt.</a>

## zuluCrypt download page

Latest released version is 7.1.0 and it can be downloaded by clicking below link.

<a href="https://github.com/mhogomchungu/zuluCrypt/releases/download/7.1.0/zuluCrypt-7.1.0.tar.xz">zuluCrypt-7.1.0.tar.xz</a>

## Binary packages

Ubuntu, Fedora and opensuse users can install binary packages by following instructions on <a href="http://software.opensuse.org/download.html?project=home%3Aobs_mhogomchungu&package=zulucrypt">this link</a>.

zuluCrypt packages are also in Fedora, Debian and Ubuntu repositories and can be installed through normal/standard package installation procedures in those distributions.

```
zuluCrypt packages i provide are better than those provided by these distributions
because of the following reasons:

1. My packages do not generated a polkit prompt that requires a root password
   when GUI components are started.

2. My packages provides CLI components that work from a normal user account.
```


```Archlinux``` users can get the latest version of zuluCrypt from <a href="https://aur.archlinux.org/packages/zulucrypt/">here</a>.

```Archlinux``` users can get the development version of zuluCrypt from <a href="https://aur.archlinux.org/packages/zulucrypt-git/">here</a>.

## cryptsetup

<a href="https://gitlab.com/cryptsetup/cryptsetup/blob/master/README.md">cryptsetup</a> is one of the most popular linux native solutions for block device encryption.

## zuluplay

<a href="https://github.com/mhogomchungu/zuluplay">zuluplay</a> is an independent, BSD licensed full implementation of truecrypt/veracrypt formatted encrypted volumes.


## VeraCrypt

<a href="https://www.veracrypt.fr/en/Home.html">VeraCrypt</a> is a security enhanced fork of TrueCrypt.


## BitLocker

BitLocker is a full volume encryption feature included with Microsoft Windows versions starting with Windows Vista.

## Where did the name come from?

The "crypt" part is there to show core feature of the tool. The "zulu" part come from "zulu time", commonly known as "GMT" (Greenwich Mean Time). The tv was on as i was thinking of what name to use for the project and somebody on tv kept mentioning "zulu time" (i think it was a military related show) and the zulu part of the name got stuck.

## Various components of the tools

zuluCrypt is made up of two components: The back end, CLI is called "zuluCrypt-cli". It is build using C, has required dependencies on libblkid, libcryptsetup and an optional dependency on libtcplay. The front end is a Qt GUI application called "zuluCrypt-gui". It is build using C++/Qt.

zuluMount is made up of two components: The back end, CLI is called "zuluMount-cli". It is build using C and has a dependency on zuluCrypt-cli libraries. The front end is a Qt GUI application called "zuluMount-gui". It is build using C++/Qt.

The GUI components are optional and the project can be build without them. They have a run-time optional dependency on <a href="http://linux.die.net/man/1/xdg-open">"xdg-open"</a>. If present and working properly, a default file manager will open a mount point folder automatically when a volume is opened/mounted.

## Frequently asked questions
The list of most frequently asked questions can be found <a href="https://github.com/mhogomchungu/zuluCrypt/wiki/Frequently-Asked-Questions">here</a>.


## zuluCrypt screenshots

<a href="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_121900.png" target="_blank"><img src="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_121900.png"></a>

<a href="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_121934.png" target="_blank"><img src="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_121934.png"></a>

<a href="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_122017.png" target="_blank"><img src="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_122017.png"></a>


## zuluMount screenshot

<a href="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_122040.png" target="_blank"><img src="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_122040.png"></a>

<a href="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_122108.png" target="_blank"><img src="https://github.com/mhogomchungu/zuluCrypt/raw/master/images/Screenshot_20190912_122108.png"></a>
## Contact info:

last update: Tue Dec  3 09:23:35 AM EAT 2024
