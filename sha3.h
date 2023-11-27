// sha3.h
// 19-Nov-11  Markku-Juhani O. Saarinen <mjos@iki.fi>

#ifndef SHA3_H
#define SHA3_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef KECCAKF_ROUNDS
#define KECCAKF_ROUNDS 24
#endif

#ifndef ROTL64
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

// state context
typedef struct {
    union {
        uint8_t msg_bytes[200];
        uint64_t state[25];
    } st;
    int end_rate, rate, mode;
} sha3_ctx_t;

// Compression function.
void sha3_keccakf_sponge(uint64_t state[25]);

// OpenSSL - like interfece
int sha3_init(sha3_ctx_t *c, int mode);    // mode = hash output in bytes
int keccak_absorb(sha3_ctx_t *c, const void *message, size_t lenMessage);
void keccak_pad(sha3_ctx_t *c); // Apply padding
int keccak_squeeze(void *hash, sha3_ctx_t *c);    // digest goes to hash

// compute a sha3 hash (hash) of given byte length from "message"
void *sha3(const void *message, size_t lenMessage, void *hash, int mode, bool shake);

// SHAKE128 and SHAKE256 extensible-output functions
void shake_xof(sha3_ctx_t *c);
void shake_out(sha3_ctx_t *c, void *out, size_t len);

#endif

