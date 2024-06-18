#ifndef XHAMMING_H
#define XHAMMING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"
#define BLUE "\x1B[34m"

#define MAX_FILENAME 256


// Devuelve la cantidad de bit de control que deberia tener un bloque de hamming
int get_parity_bit_count(int n) {
    int k = 0;
    while ((1 << k) < n + k + 1) {
        k++;
    }
    return k;
}

// Calcula los bits de paridad y configura en el array 'data'
//                                        8       3
void calculate_parity_bits(char *data, int n, int k) {
    for (int p = 0; p < k; p++) {
        int parity = 0;
        int parity_pos = (1 << p) - 1;
        for (int i = parity_pos; i < n; i += (2 << p)) {
            for (int j = i; j < i + (1 << p) && j < n; j++) {
                parity ^= get_bit(data, j);
            }
        }
        set_bit(data, parity_pos, parity);
    }
}

void calculate_block_parity(char *block, int block_size) {
    int parity = 0;

    // Calcular la paridad XOR de todos los bits en el bloque
    for (int i = 0; i < block_size - 1; i++) {
        parity ^= get_bit(block, i);
    }

    // Establecer el bit de paridad al final del bloque
    set_bit(block, block_size - 1, parity);
}

size_t calcularNullRestantes(size_t size, size_t block_size){
    return block_size - (size % block_size);
}

int protect_file(const char *input_filename, int data_size, const char *output_filename) {
    printf("Protegiendo!\n");
    pthread_t loader_thread;
    pthread_create(&loader_thread, NULL, loader, NULL);

    size_t size;
    char *data = load_file2(input_filename, &size); // Carga los datos a codificar
    if (!data) {
        loading = false;
        pthread_join(loader_thread, NULL);
        return 1;
    }

    // INICIO SHIFTEO MENSAJE
    size_t oldSize = size;
    size = size + sizeof(size_t);
    //printf("\nSize a Guardar: %d", oldSize);

    size_t total_bits = size * 8;
    int k = get_parity_bit_count(data_size);
    int n = data_size;
    int block_size = n + k + 1;
    int block_bytes = (block_size + 7) / 8;
    //printf("\nn, k: (%i, %i), blocksize: %i ", n, k, block_size);
    //printf("\ntotal bits: %d\n", total_bits);

    // Preparar archivo de Salida
    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error creating protected file");
        free(data);
        loading = false;
        pthread_join(loader_thread, NULL);
        return 2;
    }

    // Pide memoria para un unico bloque
    //printf("\nBlock Size in Bytes: %d: \n", block_bytes);
    char *block = malloc(block_bytes);
    if (!block) {
        perror("Memory allocation failed for block");
        fclose(out);
        free(data);
        loading = false;
        pthread_join(loader_thread, NULL);
        return 3;
    }

    for (int i = 0; i < total_bits; i += n) {
        memset(block, 0, block_bytes);

        int bitPosACopiar = 0;
        for (int j = 0; j < n + k; j++) {
            if (i + bitPosACopiar > size * 8) break;

            if (!isPowerOfTwo(j + 1)) {
                set_bit(block, j, get_bit(data, i + bitPosACopiar));
                bitPosACopiar++;
            }
        }

        calculate_parity_bits(block, block_size, k);
        calculate_block_parity(block, block_size);
        fwrite(block, 1, block_bytes, out);
    }

    fclose(out);
    free(block);
    free(data);

    loading = false;
    pthread_join(loader_thread, NULL);
    return 0;
}


// Dado el nombre de un archivo, devuelve en entero el tipo de bloque de hamming que es
int getExtensionIndex(char* input_filename){
    int len = strlen(input_filename);
    char indexChar = input_filename[len-1];


    // Para el uso de atoi
    char indexString[2];
    indexString[0] = indexChar;
    indexString[1] = '\0';

    int index = atoi(indexString);
//    printf("%c %d\n",indexChar,index);

    return index;
}

// Dado el nombre del archivo, devuelve el tamaño del bloque de hamming

// Dado el tipo de bloque, retorna el tamaño de los datos
int getBlockSizeByIndex(int n){
    switch(n){
        case 1:
            return 4;
        case 2:
            return 4083;
        case 3:
            return 65519;
        default:
            return 4;
    }
}
int getBlockSizeByExtension(char* input_filename){
    return getBlockSizeByIndex(getExtensionIndex(input_filename));
}

// Dado el tamaño del bloque, retorna su codificacion para la extension del archivo protegido
int getIndexForExtension(int n){
    switch(n){
        //7
        case 4:
            return 1;
        //4095
        case 4083:
            return 2;
        //65535
        case 65519:
            return 3;
        default:
            return 1;
    }
}

int introduce_errors(char *input_filename, char *output_filename) {
    printf("Introduciendo errores! \n");
    pthread_t loader_thread;
    loading = true;
    pthread_create(&loader_thread, NULL, loader, NULL);

    size_t size;
    char *data = load_file(input_filename, &size);
    if (!data) {
        loading = false;
        pthread_join(loader_thread, NULL);
        return 1;
    }

    // Calcula el tamaño del bloque en base a la extension
    int data_size = getBlockSizeByExtension(input_filename);
    int k = get_parity_bit_count(data_size);
    int n = data_size;
    int block_size = n + k + 1;

    //

    // Calcular extension archivo de salida
    char indexString[5];
    sprintf(indexString, "%d", getIndexForExtension(data_size));
    strcat(output_filename, indexString);

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo con errores");
        free(data);
        loading = false;
        pthread_join(loader_thread, NULL);
        return 2;
    }

    // Introducir un error aleatorio por bloque
    srand(time(NULL)); // Inicialización del generador de números aleatorios
    for (int i = 0; i < size * 8; i += block_size) {
        int errorPosition = rand() % block_size;
        set_bit(data, i + errorPosition, !get_bit(data, i + errorPosition));
    }

    fwrite(data, 1, size, out);
    fclose(out);
    free(data);

    loading = false;
    pthread_join(loader_thread, NULL);
    return 0;
}

int correct_error(char *data, int block_size, size_t size) {
    int block_bytes = (block_size - 1 + 7) / 8;

    for (int i = 0; i < size; i += block_bytes) {
        int index = 0;
        int parity = 0;
        int pos = 0;
        for (int j = 0; j < block_size - 1; j++) {
            pos = i * 8 + j;
            if (get_bit(data, pos)) {
                index ^= j + 1;
                parity ^= 1;
            }
        }
        if (get_bit(data, pos + 1)) parity ^= 1;
        if (index) {
            if (parity) {
                // Paridad impar, un solo error
                index -= 1;
                set_bit(data, i * 8 + index, !get_bit(data, i * 8 + index));
            } else {
                return 1; // Retornar indicando que hubo un error que no se pudo corregir
            }
        }
    }
    return 0;
}

int decode_file(const char *input_filename, const char *output_filename, int block_size, int correct_errors) {
    printf("Decodificando!\n");
    pthread_t loader_thread;
    loading = true;
    pthread_create(&loader_thread, NULL, loader, NULL);

    size_t size;
    char *data = load_file(input_filename, &size);
    if (!data) {
        loading = false;
        pthread_join(loader_thread, NULL);
        return 1;
    }

    int parity_bits = get_parity_bit_count(block_size); // nro bits de control
    int data_bits = block_size; // nro bits de info
    block_size = data_bits + parity_bits + 1;
    int data_bytes = data_bits / 8 + (data_bits % 8 != 0); // técnica de redondeo hacia arriba

    int block_bytes = (block_size + 7) / 8;

    int numberOfBlocks = size / block_bytes;
    int totalDataBits = numberOfBlocks * data_bits;
    int totalDataBytes = totalDataBits / 8;

    //printf("%d,%d,%d,%d,%d, %d, %d\n", parity_bits, data_bits, data_bytes, numberOfBlocks, block_size, totalDataBytes, totalDataBits);

    // Pide espacio para la cadena de caracteres decodificada
    char *output_data = malloc(size);
    memset(output_data, 0, size);

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado");
        free(data);
        loading = false;
        pthread_join(loader_thread, NULL);
        return 2;
    }

    if (correct_errors) {
        correct_error(data, block_size, size);
    }

    int output_index = 0;
    for (int i = 0; i < size; i += block_bytes) {
        for (int j = 0; j < block_size; j++) {
            if (output_index > size * 8) break; // Condición de parada por si se excede

            // Copia los bits de data en output_data eliminando los bits de control
            if (!isPowerOfTwo(j + 1)) {
                int bitToCopy = get_bit(data, i * 8 + j);
                set_bit(output_data, output_index, bitToCopy);
                output_index++;
            }
        }
    }

    size_t decompressedSize;
    memccpy(&decompressedSize, output_data, 1, sizeof(size_t)); // se recupera bien ?
    //printf("Size recuperado: %llu\n", decompressedSize);

    // Escribir los datos decodificados al archivo, considerando los bits efectivamente utilizados
    fwrite(output_data + sizeof(size_t), 1, decompressedSize, out); // Write the actual bytes used

    fclose(out);
    free(data);
    free(output_data);

    loading = false;
    pthread_join(loader_thread, NULL);
    return 0;
}

// Introduce errores aleatorios en el archivo
int introduce_two_errors(char *input_filename, char *output_filename) {
    size_t size;
    char *data = load_file(input_filename, &size);
    if (!data) return 1;

    // Calcula el tamaño del bloque en base a la extension
    int data_size = getBlockSizeByExtension(input_filename);
    int k = get_parity_bit_count(data_size);
    int n = data_size;
    int block_size = n + k + 1;

    printf("size in bytes: %d, block_size: %d\n",size,block_size);

    // Calcular extension archivo de salida
    char indexString[5];
    sprintf(indexString, "%d", getIndexForExtension(data_size));
    strcat(output_filename,indexString);

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo con errores");
        free(data);
        return 2;
    }

    // Introducir un error aleatorio por bloque
    srand(time(NULL)); // Inicialización del generador de números aleatorios
    for(int i = 0; i < size*8; i+=block_size){
        int errorPosition = rand() % block_size;
//        printf("%d %d\n",errorPosition,block_size);
//        printf("Index iter: %d\n",i);
//        printBlock(data,block_size);

        set_bit(data,i+errorPosition,!get_bit(data,i+errorPosition));


        int errorPosition2 = rand() % block_size;
        while(errorPosition2 == errorPosition){
            errorPosition2 = rand() % block_size;
        }
        set_bit(data,i+errorPosition2,!get_bit(data,i+errorPosition2));


//        printBlock(data,block_size);
    }

    fwrite(data, 1, size, out);
    fclose(out);
    free(data);
    return 0;
}
#endif // XHAMMING_H