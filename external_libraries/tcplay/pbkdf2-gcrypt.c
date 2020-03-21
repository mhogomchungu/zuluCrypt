/*
 * Copyright (c) 2011 Alex Hornung <alex@alexhornung.com>.
 * All rights reserved.
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

#include <errno.h>
/*
 * Yey for gcrypt and its broken includes...
 * see http://lists.gnupg.org/pipermail/gcrypt-devel/2011-July/001830.html
 * and http://seclists.org/wireshark/2011/Jul/208
 * for more details...
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#include "tcplay.h"

extern int zuluCryptIterationCount ;

static
int
get_gcrypt_hash_id(struct pbkdf_prf_algo *hash)
{
	if	(strcmp(hash->algo, "RIPEMD160") == 0)
		return GCRY_MD_RMD160;
	else if (strcmp(hash->algo, "SHA512") == 0)
		return GCRY_MD_SHA512;
	else if (strcmp(hash->algo, "SHA256") == 0)
		return GCRY_MD_SHA256;
	else if	(strcmp(hash->algo, "whirlpool") == 0)
		return GCRY_MD_WHIRLPOOL;
	else
		return -1;
}

int
pbkdf2(struct pbkdf_prf_algo *hash, const char *pass, int passlen,
    const unsigned char *salt, int saltlen,
    int keylen, unsigned char *out)
{
	gpg_error_t err;

	int iter ;

	if (zuluCryptIterationCount)
		iter = zuluCryptIterationCount ;
	else
		iter = hash->iteration_count ;

	err = gcry_kdf_derive(pass, passlen, GCRY_KDF_PBKDF2,
	    get_gcrypt_hash_id(hash),
	    salt, saltlen, iter, keylen, out);

	if (err) {
		tc_log(1, "Error in PBKDF2\n");
		return EINVAL;
	}

	return 0;
}

