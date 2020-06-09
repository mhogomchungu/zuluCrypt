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

#ifndef _TCPLAY_API_H
#define _TCPLAY_API_H

#include <stddef.h>

#define TC_OK		0
#define TC_ERR		-1
#define TC_ERR_UNIMPL	-255

#define TC_STATE_ENTER	1
#define TC_STATE_EXIT	0

struct _tc_api_task;

typedef struct _tc_api_task *tc_api_task;

typedef int (*tc_api_cipher_iterator_fn)(void *, const char *, int /* klen */, int /* length */);
typedef int (*tc_api_prf_iterator_fn)(void *, const char *);
typedef int (*tc_api_state_change_fn)(void *, const char *, int);

#ifdef __cplusplus
extern "C" {
#endif

int tc_api_init(int verbose);
int tc_api_uninit(void);

int tc_api_has(const char *feature);

int tc_api_cipher_iterate(tc_api_cipher_iterator_fn fn, void *priv);
int tc_api_prf_iterate(tc_api_prf_iterator_fn fn, void *priv);


tc_api_task tc_api_task_init(const char *op);
int tc_api_task_uninit(tc_api_task task);
int tc_api_task_set(tc_api_task task, const char *key, ...);
int tc_api_task_do(tc_api_task task);

int tc_api_task_info_get(tc_api_task task, const char *key, ...);
const char *tc_api_task_get_error(tc_api_task task);

#ifdef __cplusplus
}
#endif

#endif
