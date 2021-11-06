#ifndef SECURITY_H
#define SECURITY_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include "sha256.h"
#include "aes.h"

#define ENCRYPT 1
#define DECRYPT 0

void digest(unsigned char *data, unsigned char *key, unsigned char *final);
int verify_digest(unsigned char *data, unsigned char* key, unsigned char* dig);
void AES(struct AES_ctx* ctx, uint8_t *data, int len);
struct AES_ctx AES_init(const uint8_t *key, const uint8_t *iv);

#endif   // SECURITY_H