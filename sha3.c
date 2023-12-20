#include <stdio.h>
#include "sha3.h"

// constants for keccak permutation
const uint64_t keccakf_rndc[KECCAKF_ROUNDS] = {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};
const int keccakf_rotc[KECCAKF_ROUNDS] = {
        1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
        27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};
const int keccakf_piln[KECCAKF_ROUNDS] = {
        10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
};




// Step Mapping
// KECCAK-f[b] = KECCAK-p[b, 12+2l] => KECCAK-f[1600] = KECCAK-p[1600, 24]

// state : array of [5][5] in single for better perf
void sha3_keccakf_sponge(uint64_t state[25])
{

    // variables
    int x, y, r,t;
    uint64_t D, C[5];


    // actual iteration
    for (r = 0; r < KECCAKF_ROUNDS; r++) {

        // The θ routine

        // Step 1 : Get C
        for (x = 0; x < 5; x++)
            C[x] = state[x] ^ state[x + 5] ^ state[x + 10] ^ state[x + 15] ^ state[x + 20];

        // Step 2 : Get D
        for (x = 0; x < 5; x++) {
            D = C[(x + 4) % 5] ^ ROTL64(C[(x + 1) % 5], 1);

            // Step 3 : Compute state
            for (y = 0; y < 25; y += 5)
                state[y + x] ^= D;
        }

        // La routine ρ
        // La routine π
        // Permutation des 25 mots avec un motif fixé:
        //  a[3i+2j][i] = a[i][j]

        D = state[1];
        for (t = 0; t < 24; t++) {
            y = keccakf_piln[t];
            C[0] = state[y];
            state[y] = ROTL64(D, keccakf_rotc[t]);
            D = C[0];
        }

        // La routine χ
        for (y = 0; y < 25; y += 5) {
            for (x = 0; x < 5; x++)
                C[x] = state[y + x];
            for (x = 0; x < 5; x++)
                state[y + x] ^= (~C[(x + 1) % 5]) & C[(x + 2) % 5];
        }

        //  Iota
        state[0] ^= keccakf_rndc[r];
    }
}


// Absorb more data into the Keccak state
// Converting bytes to State Arrays
int keccak_absorb(sha3_ctx_t *c, const void *message, size_t lenMessage)
{
    size_t i;
    int j;

    j = 0;
    for (i = 0; i < lenMessage; i++) {
        // The message (input data) is XORed with the current bytes of the block
        c->st.msg_bytes[j++] ^= ((const uint8_t *) message)[i];

        // When the block is full (reaches the rate size), the algorithm proceeds to the permutation step.
        if (j >= c->rate) {
            sha3_keccakf_sponge(c->st.state);
            j = 0;
        }
    }
    c->end_rate = j;

    return 1;
}

// Pad with 10^*1 to make input length multiple of rate.
void keccak_pad(sha3_ctx_t *c) {
    // Padding is added to the last block of data
    c->st.msg_bytes[c->end_rate] ^= 0x06; // 00000110
    c->st.msg_bytes[c->rate - 1] ^= 0x80; // 10000000
}


// finalize and output a hash

int keccak_squeeze(void *hash, sha3_ctx_t *c)
{
    int i;
    // printf("c msg bytes : %s\n", c->st.msg_bytes);

    // Apply Keccak-f permutation
    sha3_keccakf_sponge(c->st.state);

    // Output hash
    for (i = 0; i < c->mode; i++) {
        ((uint8_t *) hash)[i] = c->st.msg_bytes[i];
    }

    return 1;
}

// compute a SHA-3 hash (hash) of given byte length from "message"
// mode = hash output in bytes
void *sha3(const void *message, size_t lenMessage, void *hash, int mode)
{
    sha3_ctx_t sha3;
    int i;

    // Init state array
    for (i = 0; i < 25; i++)
        sha3.st.state[i] = 0;
    sha3.mode = mode;
    sha3.rate = 200 - 2 * mode; // In bytes like 1600-capacity
    sha3.end_rate = 0;
    printf("Mode : %i\nrate : %i\ncapacity : %i\n",mode, sha3.rate, 2 * mode);

    // Sponge part
    keccak_absorb(&sha3, message, lenMessage);

//    printf("sha3 msg bytes : %s\n", sha3.st.msg_bytes);
//    printf("sha3 state : %s\n", sha3.st.state);
//    printf("end_rate %i\n", sha3.end_rate);

    keccak_pad(&sha3);
    keccak_squeeze(hash, &sha3);

    return hash;
}
