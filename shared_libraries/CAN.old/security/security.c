#include "security.h"
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "des.h"

void digest(unsigned char *data, unsigned char* key, unsigned char *final){
    SHA256_CTX x1;
    unsigned char new_data[24];

    // Digest fatto su K | M | K
    int i=0;
    for(i=0; i<8; i++)
        new_data[i] = key[i];
    for(i=8; i<16; i++)
        new_data[i] = data[i];
    for(i=16; i<24; i++)
        new_data[i] = key[i];

    sha256_init(&x1);
    sha256_update(&x1, new_data, 24);
    sha256_final(&x1, final);

}

int verify_digest(unsigned char *data, unsigned char* key, unsigned char* dig){
    unsigned char final[32];
		int i;
    digest(data, key, final);
		

    for(i = 0; i<32; i++){
        if(final[i] != dig[i])
            return 0;
    }

    return 1;
}

void AES(struct AES_ctx* ctx, uint8_t *data){
    AES_CTR_xcrypt_buffer(ctx, data, 16);
}

struct AES_ctx AES_init(const uint8_t *key, const uint8_t *iv){
    struct AES_ctx aes;
    AES_init_ctx_iv(&aes, key, iv);
    return aes;
}
