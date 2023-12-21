#ifndef SHA3_H
#define SHA3_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define KECCAKF_ROUNDS 24

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

// state context
typedef struct {
    union {
        uint8_t msg_bytes[200];
        uint64_t state[25];
    } st;
    int end_rate, rate, mode;
} sha3_ctx_t;


void sha3_keccakf_sponge(uint64_t state[25]);
int keccak_absorb(sha3_ctx_t *c, const void *message, size_t lenMessage);
void keccak_pad(sha3_ctx_t *c); // Apply padding
int keccak_squeeze(void *hash, sha3_ctx_t *c);    // digest goes to hash
void shake_xof(sha3_ctx_t *c);
void shake_squeeze(sha3_ctx_t *c, void *hash, size_t output_lengh);

// compute a sha3 hash (hash) of given byte length from "message"
void *sha3(const void *message, size_t lenMessage, void *hash, int mode, bool shake, int output_lengh);


#endif

