// main.c
// 19-Nov-11  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "sha3.h"


// Function to convert a byte to its hexadecimal representation
void byte_to_hex(unsigned char byte, char* hex) {
    sprintf(hex, "%02X", byte);
}

// Function to read the entire content of a file as hexadecimal
char* read_file_as_hex(const char* file_path, size_t* hex_size) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // Allocate memory for the hexadecimal string
    char* hex_string = (char*)malloc(file_size * 2 + 1); // Each byte requires two hexadecimal characters
    if (!hex_string) {
        perror("Memory allocation error");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Read the file content as bytes and convert to hexadecimal
    size_t index = 0;
    int byte;
    while ((byte = fgetc(file)) != EOF) {
        byte_to_hex((unsigned char)byte, &hex_string[index]);
        index += 2;
    }

    // Add null terminator
    hex_string[index] = '\0';

    // Set the size of the hexadecimal string
    *hex_size = index;

    fclose(file);
    return hex_string;
}



char* bytes_to_hex_string(const unsigned char* bytes, size_t size) {
    // Allocate memory for the hexadecimal string (twice the size of the byte array + 1 for null terminator)
    char* hex_string = (char*)malloc(size * 2 + 1);
    if (!hex_string) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    // Convert each byte to two hexadecimal characters
    for (size_t i = 0; i < size; i++) {
        sprintf(hex_string + 2 * i, "%02X", bytes[i]);
    }

    // Add null terminator
    hex_string[size * 2] = '\0';

    return hex_string;
}

// read a hex string, return byte length or -1 on error.

static int test_hexdigit(char ch)
{
    if (ch >= '0' && ch <= '9')
        return  ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return  ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return  ch - 'a' + 10;
    return -1;
}

static int test_readhex(uint8_t *buf, const char *str, int maxbytes)
{
    int i, h, l;

    for (i = 0; i < maxbytes; i++) {
        h = test_hexdigit(str[2 * i]);
        if (h < 0)
            return i;
        l = test_hexdigit(str[2 * i + 1]);
        if (l < 0)
            return i;
        buf[i] = (h << 4) + l;
    }

    return i;
}


// Fonction pour lire le contenu complet d'un fichier en tant que tableau d'octets
unsigned char* read_file(const char* file_path, size_t* size) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Déterminer la taille du fichier
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    // Allouer de la mémoire pour le contenu du fichier
    unsigned char* content = (unsigned char*)malloc(*size);
    if (!content) {
        perror("Erreur lors de l'allocation de mémoire");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Lire le contenu du fichier
    size_t bytesRead = fread(content, 1, *size, file);
    if (bytesRead != *size) {
        perror("Erreur lors de la lecture du fichier");
        free(content);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return content;
}

// test speed of the comp

void test_speed()
{
    int i;
    uint64_t st[25], x, n;
    clock_t bg, us;

    for (i = 0; i < 25; i++)
        st[i] = i;

    bg = clock();
    n = 0;
    do {
        for (i = 0; i < 100000; i++)
            sha3_keccakf_sponge(st);
        n += i;
        us = clock() - bg;
    } while (us < 3 * CLOCKS_PER_SEC);

    x = 0;
    for (i = 0; i < 25; i++)
        x += st[i];

    printf("(%016lX) %.3f Keccak-p[1600,24] / Second.\n",
        (unsigned long) x, (CLOCKS_PER_SEC * ((double) n)) / ((double) us));


}

// main
int main(int argc, char **argv)
{

// Init
    int i, digest_in_bytes, digest;
    size_t bytes_size;
    unsigned char* bytes;
    const char* file_path;
    bool shake;
    uint8_t buf[64];

    i = 1;
    digest = 256;
    shake = false;

    // Get mode, by default sha3-256
    if (argc > 2 && strcmp(argv[1], "-m") == 0) {
        digest = atoi(argv[2]);
        if (!(digest == 224 || digest == 256 || digest == 384 || digest == 512)) {
            printf("Error : mode 224 or 256 or 384 or 512\n");
            exit(1);
        }
        i = 3;
    }
    else if (argc > 2 && strcmp(argv[1], "-s") == 0) {
        digest = atoi(argv[2]);
        if (!(digest == 128 || digest == 256)) {
            printf("Error : shake mode 128 or 256\n");
            exit(1);
        }
        i = 3;
        shake = true;
    }

    digest_in_bytes = digest / 8;

    for (; i < argc; i++) {
        file_path = argv[i];
        printf("Opening file %s ...\n", file_path);
        bytes = read_file(file_path, &bytes_size);
        memset(buf, 0, sizeof(buf));

        printf("Starting sha3...\n");
        sha3(bytes, bytes_size, buf, digest_in_bytes, false);

        printf("Sha3-%i result : %s\n", digest, bytes_to_hex_string(buf, digest_in_bytes));
    }

//    if (test_sha3() == 0)// && test_shake() == 0)
//        printf("FIPS 202 / SHA3, SHAKE128, SHAKE256 Self-Tests OK!\n");
//    test_speed();

    return 0;

}

