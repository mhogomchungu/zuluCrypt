/*
 *
 *  Copyright (c) 2012
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../zuluCrypt-cli/constants.h"
#include "../zuluCrypt-cli/bin/libzuluCrypt-exe.h"
#include "../zuluCrypt-cli/bin/includes.h"
#include "../zuluCrypt-cli/lib/libzuluCrypt.h"
#include "../zuluCrypt-cli/lib/includes.h"
#include "../zuluCrypt-cli/bin/includes.h"

typedef struct{
	const char * device     ;
	const char * action     ;
	const char * m_point    ;
	const char * m_opts     ;
	const char * key        ;
	const char * key_source ;
	const char * fs_opts    ;
	const char * uuid       ;
	const char * type       ;
	const char * offset     ;
	const char * tcrypt_multiple_keyfiles[ TRUECRYPT_MAX_KEYFILES + 1 ] ;
	stringList_t env        ;
	int          mpo        ;
	uid_t        uid        ;
	int          share      ;
}ARGS;

int zuluMountVolumeStatus( const char * device,const char * UUID,uid_t uid ) ;

int zuluMountUnEncryptedVolumeStatus( const char * device ) ;

int zuluMountPrintVolumesProperties( uid_t uid ) ;

int zuluMountprintAListOfMountedVolumes( void ) ;

int zuluMountPrintDeviceProperties( const char * device,const char * UUID,uid_t uid ) ;

int zuluMountUMount( ARGS * args ) ;

int zuluMountMount( ARGS * args ) ;

int zuluMountCryptoMount( ARGS * args ) ;

int zuluMountCryptoUMount( ARGS * args ) ;

int _zuluExit( int st,string_t z,char * q,const char * msg ) ;

int _zuluExit_1( int st,stringList_t z,char * q,const char * msg ) ;

void zuluMountPartitionProperties( const char * mapper,const char * UUID,const char * device,const char * m_point ) ;
