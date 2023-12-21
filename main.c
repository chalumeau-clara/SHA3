#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "sha3.h"


// Function to convert a byte to its hexadecimal representation
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


// main
int main(int argc, char **argv)
{

// Init
    int i, digest_in_bytes, digest;
    size_t bytes_size;
    unsigned char* bytes;
    const char* file_path;
    uint8_t buf[6400];
    bool shake;
    int output_lengh;

    i = 1;
    digest = 256;
    shake = false;
    output_lengh = 32;

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
        if (argc > 4 && strcmp(argv[3], "-o") == 0) {
            output_lengh = atoi(argv[4]) / 8;
            i +=2;
            printf("output lengh : %i\n", output_lengh);
        }
        digest = atoi(argv[2]);
        if (!(digest == 128 || digest == 256)) {
            printf("Error : shake mode 128 or 256\n");
            exit(1);
        }
        i += 2;
        shake = true;
    }


    digest_in_bytes = digest / 8;

    for (; i < argc; i++) {
        file_path = argv[i];
        printf("Opening file %s ...\n", file_path);
        bytes = read_file(file_path, &bytes_size);
        memset(buf, 0, sizeof(buf));

        printf("Starting sha3...\n");
        sha3(bytes, bytes_size, buf, digest_in_bytes, shake, output_lengh);

        printf("%s-%i result : %s\n", shake? "shake": "sha3" ,digest, bytes_to_hex_string(buf, shake ? output_lengh:digest_in_bytes));
    }

    return 0;

}

