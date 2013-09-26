/*
 * 
 *  Copyright ( c ) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "includes.h"
#include <stdio.h>
#include <libintl.h>

/*
 * The string is broken up to remove a compiler warning that says:
 * warning: string length ‘XXX’ is greater than the length ‘509’ ISO C90 compilers are required to support 
 */

void zuluCryptEXEHelp( void )
{
	const char * help1 ;
	const char * help2 ;
	const char * help3 ;
	const char * help4 ;
	const char * help5 ;
	const char * help6 ;
	const char * help7 ;
	const char * help8 ;
	const char * help9 ;
	const char * help10;
	const char * help11;
	const char * help12;
	
	help1 = gettext( "\
	usage: zuluCrypt-cli < operation > < options specific to the operation >\n\
	meaning of symbols:\n\
	[] = required option\n\
	() = optional argument,default exists\n\
	*  = default option\n\
	/ *  = alternatives for the same option\n\
	{}  = not allowed option\n\
	\n\
	zuluCrypt-cli -E [ -d ] [-e ]  [ -p/-f/-h ]\n\
	zuluCrypt-cli -D [ -d ] [-e ]  [ -p/-f/-h ]\n\
	zuluCrypt-cli -o [ -d ] [ -m ] ( -e ) [ -p/-f/-h ]\n\
	zuluCrypt-cli -O [ -d ] { -m } ( -e ) [ -p/-f/-h ]\n" ) ;
	
	help2 = gettext( "\
	zuluCrypt-cli -q [ -d ]\n\
	zuluCrypt-cli -i [ -d ]\n\
	zuluCrypt-cli -c [ -d ] [ -p/-f /-h ] ( -k ) ( -t ) ( -z  ) ( -g )\n\
	zuluCrypt-cli -r [ -d ] [ -p/-f/-h ] \n\
	zuluCrypt-cli -a [ -d ] [ [ -y/-u ] [ -l/-n ] ]/[ -h ]\n\
	zuluCrypt-cli -b [ -d ]\n\
	zuluCrypt-cli -w [ -d ]   d argument must be something like: UUID=\"2468d6a7-9a71-4312-8bd9-662f982fade5\" ( or without quotes )\n\
	zuluCrypt-cli -P [ -d ]   d device must be mapper path at /dev/mapper/\n\
	zuluCrypt-cli -X [ -d ]\n\
	zuluCrypt-cli -J [ -d ]\n" ) ;
	
	help3 = gettext( "\
	zuluCrypt-cli -R [ -d ] [ -f ]\n\
	zuluCrypt-cli -B [ -d ] [ -f ]\n\
	zuluCrypt-cli -A\n\
	zuluCrypt-cli -S\n\
	zuluCrypt-cli -N\n\
	examples:\n\
	create volume: zuluCrypt-cli -c -d /dev/sdc1 -z ext4 -t luks -p xxx\n\
	open volume  : zuluCrypt-cli -o -d /dev/sdc1 -m ~/sdc1 -e ro -p xxx\n\
	close volume ; zuluCrypt-cli -q -d /dev/sdc1\n\
	remove key   ; zuluCrypt-cli -r -d /dev/sdc1 -p xxx\n\
	add key      : zuluCrypt-cli -a -d /dev/sdc1 -y xxx -l yyy\n" ) ;
	
	help4 = gettext( "\
	get device path from mapper  : zuluCrypt-cli -P -d /dev/mapper/zuluCrypt-sdc1\n\
	check if partition with UUID is present : zuluCrypt-cli -w -d UUID=\"d2d210b8-0b1f-419f-9172-9d509ea9af0c\"\n\
	operation list\n\n\
	-c         create an encrypted volume\n\
	-o         open and encrypted volume\n\
	-O         open an encrypted volume but do not mount it( -m therefore not needed )\n\
	-q         close an opened encrypted volume\n\
	-r         remove a key from luks volume\n\
	-a         add a key to luks volume\n" ) ;
	
	help5 = gettext( "\
	-i         check if a device contain a luks volume\n\
	-s         check if a device is opened and print its properties if it is\n\
	-b         show status of each slot of luks volume.\"0\"=empty,\"1\"=occupied,\"2\"=invalid slot,\"3\"=last occupied\n\
	-A         print the list of all partitions on the system\n\
	-N         print a list of non system partitions on the system( partitions with no active entries in /etc/fstab and /etc/crypttab\n" ) ;
	
	help6 = gettext( "\
	-T         print a detailed list of mounted partitions.Must be used with -A or -S or -N\n\
	-Z         print a detailed list of unmounted partitions.Must be used with -A or -S or -N\n\
	-S         print a list of system partitions on the system( partitions with active entries in /etc/fstab and /etc/crypttab\n\
	-w         check if UUID matches UUID of any partition\n\
	-P         get device path from mapper( located at /dev/mapper )\n" ) ;
	
	help7 = gettext( "\
	-L         print a list of all opened volumes and their mount point.The list is not formatted\n\
	-X         open a device pointed by argument -d and write random data to it hiding data previously written to device\n\
	-W         check if a device is a truecrypt device or not,required argument are -p or -f\n\
	-U         print UUID of a given device,required argument: -d\n\
	-H         compare a header on a luks device to a backup header,required arg: -d and -f\n" ) ;
	
	help8 = gettext( "\
	-M         create a publicly accessible \"mirror\" of the mount point in \"/run/media/public/\" from the original\n\
		   created in \"/run/media/private/$USER/\"\n" ) ;
	help9 = gettext( "\
	-J         create a plain mapper owned by the user who run the command on a device pointed by argument -d\n\
	-B         create a luks header backup\n\
	-R         restore a luks header on a device from backup\n\
	-E         encrypt a single file\n\
	-D         decrypt a single file\n\
	\n\
	options that goes with above operations:\n\
	-G 	   module name to use to get a passphrase to open a volume\n " ) ;
	
	help10 = gettext( "\
	-k         do not ask for confirmation when doing dangerous operations\n\
	-d         path to a file or partition with encrypted volume\n\
	-m         path component to be added to mount point prefix(/run/media/private/$USER)\n\
	-z         file system type installed(ext2,ext3,ext4* etc)\n\
	-t         type of volume (plain/luks*)\n\
	-g         random number generator (/dev/random or /dev/urandom*)\n\
	-h         get passphrase interactively\n\
	-p         passphrase \n" ) ;
	
	help11 = gettext( "\
	-f         path to keyfile or luks header backup\n\
	-y         passphrase already in the volume(required by -a if -u is absent and -h is also absent)\n\
	-n         path to keyfile with a passphrase to be added (required by -a if -l is absent and -h is also absent)\n" );
	
	help12 = gettext( "\
	-u         path to keyfile with passphrase already in the volume(required by -a if -y is absent and -h is also absent)\n\
		   truecrypt hidden volume passphrase from keyfile when used with -c\n\
	-l         passphrase to be added(required by -a if -n is absent and -h is also absent)\n\
		   truecrypt hidden volume passphrase when used with -c\n\
	-e         mode for opening volumes(ro*/rw) when used with -o/-O. path to destination file when used with -E/-D\n\
		   truecrypt hidden volume size when used with -c\n" ) ;
	
	printf( "%s%s%s%s%s%s%s%s%s%s%s%s\n",help1,help2,help3,help4,help5,help6,help7,help8,help9,help10,help11,help12 ) ;
}

