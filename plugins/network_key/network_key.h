/*
 * copyright: 2014
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "crypt_buffer.h"

#include "../../zuluCrypt-cli/pluginManager/libzuluCryptPluginManager.h"
#include "../../zuluCrypt-cli/utility/socket/socket.h"
#include "../../zuluCrypt-gui/lxqt_wallet/backend/lxqtwallet.h"

#define BUFFER_SIZE 1024

struct zuluKey{
	u_int32_t wallet_key_length ;
	u_int32_t key_0_length ;
	u_int32_t key_1_length ;
	char key_0[ BUFFER_SIZE ] ;
	char key_1[ BUFFER_SIZE ] ;
	char wallet_key[ BUFFER_SIZE ] ;
	char wallet_name[ BUFFER_SIZE ] ;
	char application_name[ BUFFER_SIZE ] ;
}__attribute__((__packed__)) ;

typedef struct zuluKey zuluKey_t ;

struct zuluValue{
	int key_found ;
	size_t value_length ;
	char value[ BUFFER_SIZE ] ;
}__attribute__((__packed__)) ;

typedef struct zuluValue zuluValue_t ;

typedef struct{
	const char * path ;
	const char * uuid ;
	const char * wallet_key ;
	const char * encryption_key ;
	const char * network_address ;
	int          port_number ;
}arguments_t ;

typedef struct{

	int got_key ;
	char key[ 1024 ] ;
	size_t key_len ;

}result_t ;

const char * NETWORK_ADDRESS = "127.0.0.1" ;
int PORT_NUMBER = 40000 ;
