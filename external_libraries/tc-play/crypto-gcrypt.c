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

//#include <unistd.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

/*
 * Yey for gcrypt and its broken includes...
 * see http://lists.gnupg.org/pipermail/gcrypt-devel/2011-July/001830.html
 * and http://seclists.org/wireshark/2011/Jul/208
 * for more details...
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#include "generic_xts.h"
#include "tcplay.h"


static int
gcrypt_encrypt(void *ctx, size_t blk_len, const uint8_t *src, uint8_t *dst)
{
	gcry_cipher_hd_t cipher_hd = (gcry_cipher_hd_t)ctx;
	gcry_error_t gcry_err;

	gcry_err = gcry_cipher_encrypt(
	    cipher_hd,
	    dst,
	    blk_len, /* gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES256) */
	    src,
	    blk_len);

	return (gcry_err != 0);
}

static int
gcrypt_decrypt(void *ctx, size_t blk_len, const uint8_t *src, uint8_t *dst)
{
	gcry_cipher_hd_t cipher_hd = (gcry_cipher_hd_t)ctx;
	gcry_error_t gcry_err;

	gcry_err = gcry_cipher_decrypt(
	    cipher_hd,
	    dst,
	    blk_len /* gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES256) */,
	    src,
	    blk_len);

	return (gcry_err != 0);
}

static int
gcrypt_set_key(void **ctx, void *arg1, void *arg2 __unused, const u_int8_t *key,
    int keybits __unused)
{
	gcry_cipher_hd_t *cipher_hd = (gcry_cipher_hd_t *)ctx;
	int cipher = *((int *)arg1);
	gcry_error_t	gcry_err;

	gcry_err = gcry_cipher_open(
	    cipher_hd,
	    cipher,
	    GCRY_CIPHER_MODE_ECB,
	    0);

	if (gcry_err)
		return -1;

	gcry_err = gcry_cipher_setkey(
	    *cipher_hd,
	    key,
	    gcry_cipher_get_algo_keylen(cipher));

	if (gcry_err) {
		gcry_cipher_close(*cipher_hd);
		*ctx = NULL;
		return -1;
	}

	return 0;
}

static int
gcrypt_zero_key(void **ctx)
{
	gcry_cipher_hd_t *cipher_hd = (gcry_cipher_hd_t *)ctx;

	if (*cipher_hd == NULL)
		return 0;

	gcry_cipher_close(*cipher_hd);
	return 0;
}

static
int
get_gcrypt_cipher_id(struct tc_crypto_algo *cipher)
{
	if	(strcmp(cipher->name, "AES-128-XTS") == 0)
		return GCRY_CIPHER_AES128;
	else if (strcmp(cipher->name, "AES-256-XTS") == 0)
		return GCRY_CIPHER_AES256;
	else if (strcmp(cipher->name, "TWOFISH-128-XTS") == 0)
		return GCRY_CIPHER_TWOFISH128;
	else if (strcmp(cipher->name, "TWOFISH-256-XTS") == 0)
		return GCRY_CIPHER_TWOFISH; /* XXX: really 256? */
	else if (strcmp(cipher->name, "SERPENT-128-XTS") == 0)
		return GCRY_CIPHER_SERPENT128;
	else if (strcmp(cipher->name, "SERPENT-256-XTS") == 0)
		return GCRY_CIPHER_SERPENT256;
	else
		return -1;
}

int
syscrypt(struct tc_crypto_algo *cipher, unsigned char *key, size_t klen, unsigned char *iv,
    unsigned char *in, unsigned char *out, size_t len, int do_encrypt)
{
	struct xts_ctx *ctx;
	int cipher_id;
	int err;

	cipher_id = get_gcrypt_cipher_id(cipher);
	if (cipher_id < 0) {
		tc_log(1, "Cipher %s not found\n",
		    cipher->name);
		return ENOENT;
	}

	if ((ctx = (struct xts_ctx *)alloc_safe_mem(sizeof(struct xts_ctx))) ==
	    NULL) {
		tc_log(1, "Could not allocate safe xts_xts memory\n");
		return ENOMEM;
	}

	err = xts_init(ctx, &cipher_id, NULL, gcrypt_set_key, gcrypt_zero_key,
	    gcrypt_encrypt, gcrypt_decrypt,
	    gcry_cipher_get_algo_blklen(cipher_id),
	    key, klen);
	if (err) {
		tc_log(1, "Error initializing generic XTS\n");
		return EINVAL;
	}

	/* When chaining ciphers, we reuse the input buffer as the output buffer */
	if (out != in)
		memcpy(out, in, len);

	if (do_encrypt)
		err = xts_encrypt(ctx, out, len, iv);
	else
		err = xts_decrypt(ctx, out, len, iv);

	if (err) {
		tc_log(1, "Error encrypting/decrypting\n");
		xts_uninit(ctx);
		return EINVAL;
	}

	xts_uninit(ctx);
	free_safe_mem(ctx);

	return 0;
}

int
tc_crypto_init(void)
{
	if (!gcry_check_version(GCRYPT_VERSION)) {
		tc_log(1, "libgcrypt version mismatch\n");
		return EINVAL;
	}

	if (gcry_control(GCRYCTL_INITIALIZATION_FINISHED_P))
		return 0;

	gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
	gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
	gcry_control(GCRYCTL_RESUME_SECMEM_WARN);

	gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

	return 0;
}
