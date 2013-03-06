/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * See md5.c for more information.
 */

#ifndef _MD5_H
#define _MD5_H

/*
 * symbols are renamed to avoid collisions 
 */

/* Any 32-bit or wider unsigned integer data type will do */
typedef unsigned int zuluCryptMD5_u32plus;

typedef struct {
	zuluCryptMD5_u32plus lo, hi;
	zuluCryptMD5_u32plus a, b, c, d;
	unsigned char buffer[64];
	zuluCryptMD5_u32plus block[16];
}zuluCryptMD5_CTX;

#ifdef __cplusplus
extern "C" {
#endif
extern void zuluCryptMD5_Init( zuluCryptMD5_CTX * ctx ) ;
extern void zuluCryptMD5_Update( zuluCryptMD5_CTX * ctx,void * data,unsigned long size );
extern void zuluCryptMD5_Final( unsigned char * result,zuluCryptMD5_CTX * ctx );
#ifdef __cplusplus
}
#endif
#endif
