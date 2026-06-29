
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <time.h>

#include "minheap.h"
#include "utilidades.h"
#include "huffmanops.h"
#define MAX_FILENAME 256
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"
#define BLUE "\x1B[34m"


volatile bool loading = 1;
void *loader(void *arg) {
    const char spinner[] = "\\|/-";
    int i = 0;
    while (loading) {
        printf("\r" BLUE "Cargando... %c" RESET, spinner[i++ % 4]);
        fflush(stdout);
        usleep(100000); // 100 ms
    }
    printf("\r" GREEN "Cargando... Completado!\n" RESET);
    return NULL;
}

char* decompress_binary(char* data, size_t size, int validBitsInLastByte, MinHeapNode* root, int *finalSize) {
    MinHeapNode* search = root;
    int bitIndex = 0;
    int charIndex = 0;
    size_t maxOutputSize = size * 8; // Estimación del tamaño máximo del mensaje descomprimido
    char* msj = (char*)malloc(maxOutputSize + 1); // Alocar memoria suficiente para el mensaje original más el terminador nulo
    memset(msj, 0, maxOutputSize + 1);

    while (bitIndex < (size - 1) * 8 + validBitsInLastByte) {
        int bit = get_bit(data, bitIndex);
        bitIndex++;

        // Recorrer el árbol
        if (bit) {
            if (search->right)
                search = search->right;
        } else {
            if (search->left)
                search = search->left;
        }

        // Si llegamos a una hoja, hemos encontrado un carácter
        if (isLeaf(search)) {
            msj[charIndex] = search->data.c;
            search = root;
            charIndex++;
        }
    }
    *finalSize = charIndex;
    // Asegurarse de que el mensaje descomprimido sea nulo terminado
    msj[charIndex] = '\0';
    return msj;
}

int comprimir_huffman(char* input_filename, char* output_filename) {
    printf("Iniciando compresion!\n");
    pthread_t loader_thread;
    pthread_create(&loader_thread, NULL, loader, NULL);

    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) {
        loading = false;
        pthread_join(loader_thread, NULL);
        return 1;
    }

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado\n");
        loading = false;
        pthread_join(loader_thread, NULL);
        free(data);
        return 2;
    }

    // Calcular tabla de frecuencias
    char c[256];
    int freq[256];
    int tam = calculateFrequencyTable(data,size, c, freq);


    // Construcción del árbol de Huffman

    MinHeapNode* huffmanTreeRoot = HuffmanCodes(c, freq, tam);


    // Creación de la tabla de Huffman

    char compressionTable[256][MAX_TREE_HT];
    memset(compressionTable, 0, sizeof(char) * 256 * MAX_TREE_HT);

    char arr[MAX_TREE_HT]; // Armar pequeño acumulador para los códigos obtenidos en el barrido del árbol de Huffman
    int top = 0;
    createHuffmanTable(huffmanTreeRoot, arr, top, compressionTable);


    // Compresión del mensaje

    char* compressedMsj = malloc(sizeof(char) * size * MAX_TREE_HT);
    if(compressedMsj==NULL) {
        printf("No se pudo alocar la memoria...");
        loading = false;
        pthread_join(loader_thread, NULL);
        free(data);
        return 1;
    }
    size_t compressedMsjIndex = 0;

    for (int i = 0; i < size; i++) {
        size_t len = strlen(compressionTable[(unsigned char)data[i]]);
        memcpy(compressedMsj + compressedMsjIndex, compressionTable[(unsigned char) data[i]], len);
        compressedMsjIndex += len;
    }
    compressedMsj[compressedMsjIndex] = '\0'; // Terminar la cadena


    // Creación del árbol en orden

    huffmanData inorder[256];
    int tamano = 0;

    createInorderTree(huffmanTreeRoot, inorder, &tamano);


    // Escritura del archivo comprimido

    fwrite(&tamano, sizeof(int), 1, out);
    fwrite(inorder, sizeof(huffmanData), tamano, out); // escribe el array en un archivo

    // Convertir el mensaje comprimido de bits '0' y '1' a bytes reales
    char* binaryData;
    size_t binarySize;
    int validBitsInLastByte;
    convertToBinary(compressedMsj, &binaryData, &binarySize, &validBitsInLastByte);

    // Escribir la cantidad de bits válidos en el último byte del archivo
    fwrite(&validBitsInLastByte, sizeof(int), 1, out);
    fwrite(binaryData, sizeof(char), binarySize, out);


    // Liberar memoria y cerrar archivos
    free(binaryData);
    freeHuffmanTree(huffmanTreeRoot);
    free(data);
    free(compressedMsj);
    fclose(out);

    loading = false;
    pthread_join(loader_thread, NULL);
    return 0;
}




int descomprimir_huffman(char* input_filename, char* output_filename) {
    FILE* in = fopen(input_filename, "rb");
    printf("Iniciando descompresion!\n");
    pthread_t loader_thread;
    pthread_create(&loader_thread, NULL, loader, NULL);

    if (!in) {
        loading = false;
        pthread_join(loader_thread, NULL);
        perror("Error al abrir el archivo de entrada");
        return 1;
    }

    int tamano;
    fread(&tamano, sizeof(int), 1, in); // Leer el tamaño del array inorder

    huffmanData* inorder = (huffmanData*)malloc(tamano * sizeof(huffmanData));
    if (!inorder) {
        perror("Error al asignar memoria");
        loading = false;
        pthread_join(loader_thread, NULL);
        fclose(in);
        return 2;
    }



    fread(inorder, sizeof(huffmanData), tamano, in); // Leer el array inorder
    printf("\nInorder leido");

    /*printHuffmanData(inorder,tamano);*/
    // Reconstruir el árbol de Huffman a partir del array inorder

    MinHeapNode* huffmanTreeRoot = buildTreeFromInorder(inorder, 0, tamano-1);

    /*printf("Se armo el arbol de huffman\n");
    printHuffmanTree(huffmanTreeRoot);*/
    int validBitsInLastByte;
    fread(&validBitsInLastByte, sizeof(int), 1, in);

    // Leer el mensaje comprimido
    fseek(in, 0, SEEK_END);
    size_t fileSize = ftell(in) - sizeof(int) - (tamano * sizeof(huffmanData)) - sizeof(int);
    fseek(in, sizeof(int) + (tamano * sizeof(huffmanData)) + sizeof(int), SEEK_SET);

    char* compressedData = (char*)malloc(fileSize + 1);
    if (compressedData==NULL) {
        loading = false;
        pthread_join(loader_thread, NULL);
        perror("Error al asignar memoria");
        free(inorder);
        freeHuffmanTree(huffmanTreeRoot);
        fclose(in);
        return 3;
    }

    fread(compressedData, sizeof(char), fileSize, in);
    compressedData[fileSize] = '\0';

    // Descomprimir el mensaje
    int decompressedMessageSize;
    char* decompressedMessage = decompress_binary(compressedData, fileSize, validBitsInLastByte, huffmanTreeRoot,&decompressedMessageSize);


    // Escribir el mensaje descomprimido en el archivo de salida
    FILE* out = fopen(output_filename, "wb");
    if (out==NULL) {
        loading = false;
        pthread_join(loader_thread, NULL);
        perror("Error al crear el archivo de salida");
        free(compressedData);
        free(decompressedMessage);
        free(inorder);
        freeHuffmanTree(huffmanTreeRoot);
        return 4;
    }
    //printf(decompressedMessage);
    //printf("\n");
    /*printf("%d\n",sizeof(decompressedMessage));*/
    fwrite(decompressedMessage, sizeof(char), decompressedMessageSize, out);
    fclose(out);

    // Liberar memoria
    free(compressedData);
    free(decompressedMessage);
    free(inorder);
    freeHuffmanTree(huffmanTreeRoot);
    loading = false;
    pthread_join(loader_thread, NULL);
    return 0;
}
#endif HUFFMAN_H