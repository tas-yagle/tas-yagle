/*
 *  mbk_crypt.h
 *  avertec_distrib
 *
 *  Created by Anthony on 11/04/2008.
 *  Copyright 2008 Avertec SA. All rights reserved.
 *
 */

#include "mbk_rijndael.h"
#include MUT_H
#include AVT_H

#define KEYBITS 256

void * mbk_ascii_decrypt_thread(void *ptfiles);
int mbk_ascii_encrypt(char *inname, char *outname);
