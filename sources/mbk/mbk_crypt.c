#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#ifndef Solaris
#include <stdint.h>
#endif
#include MUT_H
#include "mbk_crypt.h"
#include "mbk_md5.h"

static const uint8_t bytetable[] = {
    0xD8,0xC4,0x49,0x53,0x6A,0x1E,0x1F,0x8E,0x0C,0x6F,0xB7,0x20,0xAE,0xE2,0xF6,0x5E,
    0x30,0xC8,0x51,0xF2,0xB9,0xD7,0xAB,0xBB,0x9D,0x79,0xEA,0x39,0xAA,0xF3,0x2B,0x25,
    0x26,0xBC,0xFC,0xE7,0x05,0xBD,0xFD,0x60,0xA3,0x86,0x5C,0xDF,0x9A,0x63,0xDA,0x3B,
    0x92,0x2C,0x3F,0x14,0x43,0x56,0xC5,0x59,0x18,0x22,0x95,0x41,0x93,0x91,0xC7,0xA1,
    0xD6,0x09,0x8A,0xF0,0x5A,0x96,0x24,0x75,0x0A,0xA4,0x7B,0x29,0xB0,0x3D,0x48,0xBF,
    0xA8,0xD0,0x82,0x6E,0xA6,0x1A,0x2A,0x7A,0x42,0xB8,0x85,0x3A,0x27,0xD9,0x98,0xED,
    0xCC,0x76,0x64,0x97,0xE1,0x02,0x69,0x7F,0xE0,0x84,0xF7,0x11,0x54,0x81,0x70,0x61,
    0x9E,0x5B,0x58,0x36,0xA7,0x00,0x06,0x5D,0x12,0x57,0x52,0x8F,0x1B,0xCF,0xC6,0x0B,
    0x44,0x15,0xE5,0x6D,0xF4,0xD1,0xAD,0x7C,0xA2,0xB5,0xA0,0x83,0xEE,0xC2,0x78,0xC0,
    0x8B,0xB4,0xDC,0x65,0xE9,0x4A,0x0F,0x40,0xC9,0xFE,0xF8,0x2F,0xEF,0x80,0x7D,0x28,
    0xA5,0xB3,0x32,0x07,0x4D,0x16,0x38,0x45,0xC3,0x74,0x35,0xEB,0x34,0x8D,0xFB,0x89,
    0x46,0x55,0x4B,0x03,0x3C,0x13,0x0D,0x62,0xDD,0xB1,0xEC,0xFA,0xA9,0x88,0xE8,0x1D,
    0x01,0xAF,0x3E,0xCE,0x72,0xDE,0x94,0x5F,0xCA,0x77,0x04,0x7E,0xB6,0xD2,0xBA,0x6C,
    0x4F,0xB2,0x2E,0xF1,0x66,0x99,0xD4,0xC1,0x68,0x31,0xE6,0x87,0xCB,0x08,0x71,0xE4,
    0x47,0xF5,0x4E,0xCD,0xD3,0x67,0x33,0x21,0xF9,0x73,0x19,0x50,0x8C,0xDB,0x9B,0x90,
    0xBE,0x9C,0x9F,0x37,0x4C,0x17,0x10,0xD5,0x23,0x2D,0xAC,0x6B,0x1C,0xE3,0x0E,0xFF,
};

static char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char base64inv[256];

static int encrypt_block(uint32_t *rk, int nrounds, unsigned char *plaintext, int count, int *pthpos, FILE *fpout);

void *
mbk_ascii_decrypt_thread(void *ptfiles)
{
    int fdout;
    FILE *fpin;
    unsigned char textbuf[65];
    unsigned char ciphertext[48];
    unsigned char decbuf[4];
    uint32_t rk[RKLENGTH(KEYBITS)];
    unsigned char key[KEYLENGTH(KEYBITS)];
    struct MD5Context md5c;
    int nrounds, i, numchars;
    const char *password;
    char *salted_password;
    uint8_t saltbyte;
    int *ptlineno;
    int c, feol, done, count, lensalt;
   
    fdout = ((decrypt_files *)ptfiles)->pipefd;
    fpin = ((decrypt_files *)ptfiles)->fp;
    ptlineno = ((decrypt_files *)ptfiles)->ptlineno;
    free(ptfiles);
    
    /* set up inverse base64 array */;
    for (i=0; i<256; i++) base64inv[i] = 0x80;
    for (i='A'; i<='Z'; i++) base64inv[i] = i-'A';
    for (i='a'; i<='z'; i++) base64inv[i] = 26+(i-'a');
    for (i='0'; i<='9'; i++) base64inv[i] = 52+(i-'0');
    base64inv['+']= 62;
    base64inv['/']= 63;
    base64inv['=']= 0;


    if (V_STR_TAB[__MBK_PASSWORD].SET) {
        password = V_STR_TAB[__MBK_PASSWORD].VALUE;
        if (strlen(password) > 128) lensalt = 128+8;
        else lensalt = strlen(password) + 8;
        salted_password = malloc(lensalt+1);
        for (i=0; i<4; i++) {
            saltbyte = bytetable[((i*3 + 3)*5) % 256];
            salted_password[i] = (char)((saltbyte != 0)?saltbyte:0x46);
        }
        if (lensalt == 136) strncpy(salted_password + 4, password, 128);
        else strcpy(salted_password + 4, password);
        for (i=lensalt-4; i<lensalt; i++) {
            saltbyte = bytetable[((i*3 + 3)*5) % 256];
            salted_password[i] = (char)((saltbyte != 0)?saltbyte:0x46);
        }
        salted_password[i] = 0;
        MD5Init(&md5c);
        MD5Update(&md5c, (unsigned char *)salted_password, lensalt);
        MD5Final(key, &md5c);
        for (i=16; i<KEYLENGTH(KEYBITS); i++) {
            key[i] = bytetable[((i*8 + 3)*7) % 256];
        }
    }
    else {
        for (i=0; i<KEYLENGTH(KEYBITS); i++) {
            key[i] = bytetable[((i*8 + 3)*7) % 256];
        }
    }
    nrounds = rijndaelSetupDecrypt(rk, key, 256);
    feol = 1;
    done = 0;
    while (!done) {
        unsigned char plaintext[16];
        
        /* read 64 encoded characters and check for end of encryption */
        count = 0;
        while (count < 64 && !done) {
            if ((c = getc(fpin)) == EOF) {
                perror("Unexpected EOF in encrypted input file");
                fclose(fpin);
                close(fdout);
                pthread_exit(NULL);
            }
            if (ferror(fpin)) {
                perror("Error reading encrypted input file");
                fclose(fpin);
                close(fdout);
                pthread_exit(NULL);
            }
            if (c == '\r' || c == '\n') {
                feol = 1;
                if (c == '\n') (*ptlineno)++;
            }
            else if (c == '.' && feol == 1) {
                done = 1;
            }
            else if (!isspace(c)) {
                textbuf[count++] = (unsigned char)c;
                feol = 0;
            }
        }
        if (count == 0 && done) break;
        textbuf[count] = 0;
        
        /* Perform base64 decoding */
        numchars = 0;
        for (i=0; i < count; i+=4) {
            decbuf[0] = base64inv[textbuf[i]];
            decbuf[1] = base64inv[textbuf[i+1]];
            decbuf[2] = base64inv[textbuf[i+2]];
            decbuf[3] = base64inv[textbuf[i+3]];
            if (decbuf[0] == 0x80 || decbuf[1] == 0x80 || decbuf[2] == 0x80 || decbuf[3] == 0x80) {
                fprintf(stderr, "[FATAL ERR] Incoherence in encrypted input\n");
                fclose(fpin);
                close(fdout);
                exit(1);
            }
            ciphertext[numchars] = (decbuf[0]<<2)|(decbuf[1]>>4);
            ciphertext[numchars+1] = (decbuf[1]<<4)|(decbuf[2]>>2);
            ciphertext[numchars+2] = (decbuf[2]<<6)|decbuf[3];
            if (textbuf[i+2] == '=') numchars ++;
            else if (textbuf[i+3] == '=') numchars += 2;
            else numchars += 3;
        }
        if (numchars != 48 && numchars != 32 && numchars != 16) {
            fprintf(stderr, "[FATAL ERR] Incoherence in encrypted input\n");
            fclose(fpin);
            close(fdout);
            exit(1);
        }

        /* Perform decryption of up to three blocks of 16 chars */
        for (i=0; i<numchars; i+=16) {
            rijndaelDecrypt(rk, nrounds, ciphertext+i, plaintext);
            write(fdout, plaintext, 16);
        }
    }
    textbuf[0] = '\n';
    textbuf[1] = '.';
    fread(textbuf+2, sizeof(unsigned char), 6, fpin);
    if (feof(fpin)) {
        perror("[FATAL ERR] Unexpected EOF in encrypted input file");
        fclose(fpin);
        close(fdout);
        exit(1);
    }
    if (ferror(fpin)) {
        perror("[FATAL ERR] Error reading encrypted input file");
        fclose(fpin);
        close(fdout);
        exit(1);
    }
    textbuf[8] = 0;
    if (strncasecmp((char *)textbuf+1, ".unprot", 7)) {
        fprintf(stderr, "[FATAL ERR] Incoherence in encrypted input\n");
        fclose(fpin);
        close(fdout);
        exit(1);
    }
    textbuf[8] = '\n';
    while ((c = getc(fpin)) != EOF && c != '\n');
    if (feof(fpin)) {
        perror("[FATAL ERR] Unexpected EOF in encrypted input file");
        exit(1);
    }
    if (ferror(fpin)) {
        perror("[FATAL ERR] Error reading encrypted input file");
        exit(1);
    }
    (*ptlineno)++;
    write(fdout, textbuf, 9);
    close(fdout);
    pthread_exit(NULL);
}

int
mbk_ascii_encrypt(char *inname, char *outname)
{
    uint32_t rk[RKLENGTH(KEYBITS)];
    unsigned char key[KEYLENGTH(KEYBITS)];
    unsigned char plaintext[49];
    char linebuf[10];
    FILE *fpin, *fpout;
    char *password;
    char *salted_password;
    uint8_t saltbyte;
    struct MD5Context md5c;
    int c, fencrypt, feol, count, nrounds, i, hpos, lensalt;

    fpin = mbkfopen(inname, NULL, "r");
    fpout = mbkfopen(outname, NULL, "w");

    if (V_STR_TAB[__MBK_PASSWORD].SET) {
        password = V_STR_TAB[__MBK_PASSWORD].VALUE;
        if (strlen(password) > 128) lensalt = 128+8;
        else lensalt = strlen(password) + 8;
        salted_password = malloc(lensalt+1);
        for (i=0; i<4; i++) {
            saltbyte = bytetable[((i*3 + 3)*5) % 256];
            salted_password[i] = (char)((saltbyte != 0)?saltbyte:0x46);
        }
        if (lensalt == 136) strncpy(salted_password + 4, password, 128);
        else strcpy(salted_password + 4, password);
        for (i=lensalt-4; i<lensalt; i++) {
            saltbyte = bytetable[((i*3 + 3)*5) % 256];
            salted_password[i] = (char)((saltbyte != 0)?saltbyte:0x46);
        }
        salted_password[i] = 0;
        MD5Init(&md5c);
        MD5Update(&md5c, (unsigned char *)salted_password, lensalt);
        MD5Final(key, &md5c);
        for (i=16; i<KEYLENGTH(KEYBITS); i++) {
            key[i] = bytetable[((i*8 + 3)*7) % 256];
        }
    }
    else {
        for (i=0; i<KEYLENGTH(KEYBITS); i++) {
            key[i] = bytetable[((i*8 + 3)*7) % 256];
        }
    }
    nrounds = rijndaelSetupEncrypt(rk, key, 256);

    fencrypt = 0;
    feol = 1;
    count = 0;
    while ((c = getc(fpin)) != EOF) {
        if (ferror(fpin)) {
            avt_errmsg(MBK_ERRMSG, "065", AVT_ERROR, strerror(errno), inname);
            fclose(fpin); fclose(fpout);
            return 1;
        }
        if (c == '\r' || c == '\n') {
            if (fencrypt) {
                plaintext[count++] = (unsigned char)c;
                if (count == 48) {
                    if (encrypt_block(rk, nrounds, plaintext, count, &hpos, fpout) != 0) {
                        avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                        fclose(fpin); fclose(fpout);
                        return 1;
                    }
                    count = 0;
                }
            }
            else if (putc(c, fpout) == EOF) {
                avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                fclose(fpin); fclose(fpout);
                return 1;
            }
            feol = 1;
        }
        else if (c == '.' && feol == 1) {
            linebuf[0] = '.';
            fgets(linebuf+1, 9, fpin);
            if (ferror(fpin)) {
                avt_errmsg(MBK_ERRMSG, "065", AVT_ERROR, strerror(errno), inname);
                fclose(fpin); fclose(fpout);
                return 1;
            }
            if (!strncasecmp(linebuf, ".PROT", 5)) {
                fencrypt = 1;
                if (linebuf[strlen(linebuf)-1] != '\n') {
                    while ((c = getc(fpin)) != EOF && c != '\n');
                    if (ferror(fpin)) {
                        avt_errmsg(MBK_ERRMSG, "065", AVT_ERROR, strerror(errno), inname);
                        fclose(fpin); fclose(fpout);
                        return 1;
                    }
                }
                if (fprintf(fpout, ".PROTECT\n") < 0) {
                    avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                    fclose(fpin); fclose(fpout);
                    return 1;
                }
                feol = 1;
                hpos = 0;
            }
            else if (!strncasecmp(linebuf, ".UNPROT", 6)) {
                fencrypt = 0;
                if (linebuf[strlen(linebuf)-1] != '\n') {
                    while ((c = getc(fpin)) != EOF && c != '\n');
                    if (ferror(fpin)) {
                        avt_errmsg(MBK_ERRMSG, "065", AVT_ERROR, strerror(errno), inname);
                        fclose(fpin); fclose(fpout);
                        return 1;
                    }
                }
                if (encrypt_block(rk, nrounds, plaintext, count, &hpos, fpout) != 0) {
                    avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                    fclose(fpin); fclose(fpout);
                    return 1;
                }
                if (hpos != 0) {
                    if (fprintf(fpout, "\n") < 0) {
                        avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                        fclose(fpin); fclose(fpout);
                        return 1;
                    }
                }
                if (fprintf(fpout, ".UNPROTECT\n") < 0) {
                    avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                    fclose(fpin); fclose(fpout);
                    return 1;
                } 
                count = 0;
                feol = 1;
            }
            else {
                if (fencrypt) {
                    for (i=0; i < (int)strlen(linebuf); i++) {
                        if (linebuf[i] == '\n' || linebuf[i] == '\r') feol = 1;
                        else feol = 0;
                        plaintext[count++] = linebuf[i];
                        if (count == 48) {
                            if (encrypt_block(rk, nrounds, plaintext, count, &hpos, fpout) != 0) {
                                avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                                fclose(fpin); fclose(fpout);
                                return 1;
                            }
                            count = 0;
                        }
                    }
                }
                else {
                    for (i=0; i < (int)strlen(linebuf); i++) {
                        if (linebuf[i] == '\n' || linebuf[i] == '\r') feol = 1;
                        else feol = 0;
                        if (putc(linebuf[i], fpout) == EOF) {
                            avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                            fclose(fpin); fclose(fpout);
                            return 1;
                        }
                    }
                }
            }
        }
        else {
            if (fencrypt) {
                plaintext[count++] = (unsigned char)c;
                if (count == 48) {
                    if (encrypt_block(rk, nrounds, plaintext, count, &hpos, fpout) != 0) {
                        avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                        fclose(fpin); fclose(fpout);
                        return 1;
                    }
                    count = 0;
                }
            }
            else if (putc(c, fpout) == EOF) {
                avt_errmsg(MBK_ERRMSG, "066", AVT_ERROR, strerror(errno), outname);
                fclose(fpin); fclose(fpout);
                return 1;
            }
            feol = 0;
        }
    }
    if (fencrypt) avt_errmsg(MBK_ERRMSG, "067", AVT_ERROR, inname);
    fclose(fpin);
    fclose(fpout);
    return 0;
}

static int
encrypt_block(uint32_t *rk, int nrounds, unsigned char *plaintext, int count, int *pthpos, FILE *fpout)
{
    unsigned char textbuf[65];
    unsigned char ciphertext[49];
    int i, pad, numchars;
    
    /* pad to multiple of 16 if necessary */
    pad = 16 - (count % 16);
    if (pad != 16) {
        for (i = 0; i < pad-1; i++) plaintext[count++] = ' ';
        plaintext[count++] = '\n';
    }

    /* Perform encryption of up to three blocks of 16 chars */
    for (i=0; i < count; i+=16) {
        rijndaelEncrypt(rk, nrounds, plaintext+i, ciphertext+i);
    }
    
    /* base 64 encode */
    numchars = 0;
    for (i=0; i < count; i+=3) {
        textbuf[numchars++] = base64chars[ciphertext[i] >> 2];
        if (i+1 >= count) {
            textbuf[numchars++] = base64chars[(ciphertext[i]&0x03)<<4];
            textbuf[numchars++] = '=';
            textbuf[numchars++] = '=';
        }
        else if (i+2 >= count) {
            textbuf[numchars++] = base64chars[((ciphertext[i]&0x03)<<4)|(ciphertext[i+1]>>4)];
            textbuf[numchars++] = base64chars[(ciphertext[i+1]&0x0f)<<2];
            textbuf[numchars++] = '=';
        }
        else {
            textbuf[numchars++] = base64chars[((ciphertext[i]&0x03)<<4)|(ciphertext[i+1]>>4)];
            textbuf[numchars++] = base64chars[((ciphertext[i+1]&0x0f)<<2)|(ciphertext[i+2]>>6)];
            textbuf[numchars++] = base64chars[ciphertext[i+2]&0x3f];
        }
    }
    textbuf[numchars] = 0;
    
    /* write block to output */
    for (i=0; i < numchars; i++) {
        if (putc(textbuf[i], fpout) == EOF) return 1;
        (*pthpos)++;
        if (*pthpos == 76) {
            if (putc('\n', fpout) == EOF) return 1;
            *pthpos = 0;
        }
    }
    return 0;
}
