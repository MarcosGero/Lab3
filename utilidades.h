/// Funciones varias para leer archivos e imprimir bits
// Carga un archivo
#pragma once

// ANSI escape codes para colores
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"


char *load_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);


    char *buffer = malloc(*size + 1);
    if (!buffer) {
        perror("Error al reservar memoria");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *size, file);
    buffer[*size] = '\0'; // Para manejo como cadena de caracteres
    fclose(file);
    return buffer;
}

char *load_file2(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    size_t total_size = sizeof(size_t) + *size + 1; // +1 para el terminador nulo
    char *buffer = malloc(total_size);
    if (!buffer) {
        perror("Error al reservar memoria");
        fclose(file);
        return NULL;
    }

    // Leer el archivo en la posición correcta dejando espacio para size_t al inicio
    fread(buffer + sizeof(size_t), 1, *size, file);
    buffer[total_size - 1] = '\0'; // Para el manejo de cadenas

    // Copiar el tamaño del archivo al principio del buffer
    memcpy(buffer, size, sizeof(size_t));

    fclose(file);
    return buffer;
}
void printChar(const char a){
    printf(" %c: ",a);
    for (int bit = 7; bit > -1; bit--) {
        if (a & (1 << bit)) {
            printf("1");
        }else{
            printf("0");
        }
    }
    printf("\n");
}

int printBlock(char *a, int size){
    for(int i =0;i<size;i++){
        printChar(a[i]);
    }
}

void printFileInBlocks(const char *input_filename, int blockSize) {
    FILE *fp = fopen(input_filename, "rb");
    if (fp == NULL) {
        printf("Error: File pointer is NULL\n");
        return;
    }

    char *buffer = (char *)malloc(blockSize * sizeof(char));
    if (buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    size_t bytesRead;
    while ((bytesRead = fread(buffer, sizeof(char), blockSize, fp)) > 0) {
        printBlock(buffer, bytesRead);
    }

    free(buffer);
}

// Comprueba si un numero es una potencia de 2
int isPowerOfTwo(int x){
    return (x & (x - 1)) == 0;
}


// Establece el bit en la posicion 'pos' (0-indexado)
void set_bit(char *array, int pos, int value) {
    int byte_pos = pos / 8;
    int bit_pos = pos % 8;
    if (value == 1)
        array[byte_pos] |= (1 << 7-bit_pos);
    else
        array[byte_pos] &= ~(1 << 7-bit_pos);
}

// Se obtiene el bit en la posicion 'pos' (0-indexado)
int get_bit(char *array, int pos) {
    int byte_pos = pos / 8;
    int bit_pos = pos % 8;
    return (array[byte_pos] >> 7-bit_pos) & 1;
}

// A utility function to print an array of size n in a buffer
void sprintArr(int arr[], int n,int* buffer)
{
    int i;
    for (i = 0; i < n; ++i)
        sprintf(buffer,"%d", arr[i]);

    sprintf(buffer,"\n");


//    sprintf(output_filename, "%s.huf", input_filename_noExtension);
}

// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void printCodes(MinHeapNode* root, int arr[],int top){

    // Nota: el recorrido es post-order

    // Assign 0 to left edge and recur
    if (root->left) {

        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    // Assign 1 to right edge and recur
    if (root->right) {

        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root)) {
        printf("%c: ", root->data);
        printArr(arr, top); // Aqui esta almacenado el codigo
    }
}

void printHuffmanData(huffmanData data[], int size) {
    for (int i = 0; i < size; ++i) {
        printf("Character: %c, Frequency: %d\n", data[i].c, data[i].freq);
    }
}
void printTreeHelper(MinHeapNode* root, int depth) {
    if (root == NULL) {
        return;
    }

    // Indentar seg�n la profundidad del nodo en el �rbol
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Imprimir el car�cter y la frecuencia del nodo actual
    printf("%c (%d)\n", root->data.c, root->data.freq);

    // Llamada recursiva para imprimir el sub�rbol izquierdo y derecho
    printTreeHelper(root->left, depth + 1);
    printTreeHelper(root->right, depth + 1);
}
void printHuffmanTree(MinHeapNode* root) {
    printTreeHelper(root, 0);
}
// Funci�n para convertir la cadena de '0' y '1' a bytes reales
void convertToBinary(char* bits, char** binaryData, size_t* binarySize, int* validBitsInLastByte) {
    size_t len = strlen(bits);
    *binarySize = (len + 7) / 8; // N�mero de bytes necesarios
    *binaryData = (char*)malloc(*binarySize);
    memset(*binaryData, 0, *binarySize);

    for (size_t i = 0; i < len; i++) {
        if (bits[i] == '1') {
            (*binaryData)[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    *validBitsInLastByte = len % 8;
    if (*validBitsInLastByte == 0) {
        *validBitsInLastByte = 8; // Si no hay padding, todos los bits en el �ltimo byte son v�lidos
    }
}

void print_size(size_t size) {
    if (size >= 1024 * 1024) {
        printf("%.2f MB", (double)size / (1024 * 1024));
    } else if (size >= 1024) {
        printf("%.2f KB", (double)size / 1024);
    } else {
        printf("%zu bytes", size);
    }
}

void calculate_compression_statistics(size_t original_size, size_t compressed_size) {
    double compression_ratio = (double)compressed_size / original_size * 100;
    printf("Tamaño original: ");
    print_size(original_size);
    printf("\n");
    printf("Tamaño comprimido: ");
    print_size(compressed_size);
    printf("\n");
    printf("Ratio de compresión: %.2f%%\n", compression_ratio);
}

void calculate_protection_statistics(size_t original_size, size_t protected_size) {
    double overhead = (double)protected_size / original_size * 100;
    printf("Tamaño original: ");
    print_size(original_size);
    printf("\n");
    printf("Tamaño protegido: ");
    print_size(protected_size);
    printf("\n");
    printf("Sobrecarga de protección: %.2f%%\n", overhead);
}
// Función para mostrar contenido en formato binario
void print_binary(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; --bit) {
            printf("%c", (data[i] & (1 << bit)) ? '1' : '0');
        }
        printf(" ");
        if ((i + 1) % 6 == 0)
            printf("\n");
    }
    printf("\n");
}

// Función para mostrar contenido en formato de caracteres
void print_chars(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%c", data[i]);
    }
    printf("\n");
}
// Función para mostrar contenido en formato hexadecimal
void print_hex(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}
// Función para mostrar contenido de dos archivos en la consola en hexadecimal
void show_files_in_console_hex(const char* file1Content, size_t file1Size, const char* file2Content, size_t file2Size) {
    printf(RED "Archivo Original (Hexadecimal):\n" RESET);
    print_hex(file1Content, file1Size);

    printf(GREEN "Archivo Comprimido (Hexadecimal):\n" RESET);
    print_hex(file2Content, file2Size);
}

// Función para mostrar contenido de dos archivos en la consola en binario
void show_files_in_console_binary(const char* file1Content, size_t file1Size, const char* file2Content, size_t file2Size) {
    printf(RED "Archivo Original (Binario):\n" RESET);
    print_binary(file1Content, file1Size);

    printf(GREEN "Archivo Comprimido (Binario):\n" RESET);
    print_binary(file2Content, file2Size);
}

// Función para mostrar contenido de dos archivos en la consola en caracteres
void show_files_in_console_chars(const char* file1Content, size_t file1Size, const char* file2Content, size_t file2Size) {
    printf(RED "Archivo Original (Caracteres):\n" RESET);
    print_chars(file1Content, file1Size);

    printf(GREEN "Archivo Comprimido (Caracteres):\n" RESET);
    print_chars(file2Content, file2Size);
}

// Crea los nombres para los archivos de salida con una extension especificada
void prepare_output_files(char* input_filename, char* input_filename_noExtension,char* output_filename,char* label,char* extension){
    printf("%s",label);
    scanf("%s", input_filename);
    strcpy(input_filename_noExtension, input_filename);
    input_filename_noExtension[strlen(input_filename) - 4] = '\0';
    sprintf(output_filename, "%s%s", input_filename_noExtension,extension);
}

void change_extension_to_huf(const char *input_filename, char *output_filename) {
    // Copiar el nombre del archivo original
    strcpy(output_filename, input_filename);
    // Encontrar el último punto en el nombre del archivo
    char *dot = strrchr(output_filename, '.');
    if (dot) {
        // Reemplazar la extensión con .huf
        strcpy(dot, ".huf");
    } else {
        // Si no hay punto, simplemente agregar .huf
        strcat(output_filename, ".huf");
    }
}

void wait_for_enter() {
    printf("Presione Enter para continuar...");
    getchar();
    getchar();
}



void change_extension_to_ha(const char *input_filename, char *output_filename1, char *output_filename2, char *output_filename3) {
    // Copiar el nombre del archivo original
    strcpy(output_filename1, input_filename);
    strcpy(output_filename2, input_filename);
    strcpy(output_filename3, input_filename);

    // Encontrar el último punto en el nombre del archivo
    char *dot1 = strrchr(output_filename1, '.');
    char *dot2 = strrchr(output_filename2, '.');
    char *dot3 = strrchr(output_filename3, '.');

    // Reemplazar la extensión con .HA1, .HA2, .HA3
    if (dot1) {
        strcpy(dot1, ".HA1");
    } else {
        strcat(output_filename1, ".HA1");
    }

    if (dot2) {
        strcpy(dot2, ".HA2");
    } else {
        strcat(output_filename2, ".HA2");
    }

    if (dot3) {
        strcpy(dot3, ".HA3");
    } else {
        strcat(output_filename3, ".HA3");
    }
}

int correct_error_and_get_positions(char *data, int block_size, size_t size, int **error_positions) {
    int parity_bits = get_parity_bit_count(block_size);
    int data_bits = block_size;
    block_size = data_bits + parity_bits + 1;

    int block_bytes = (block_size - 1 + 7) / 8;

    *error_positions = (int *)malloc(size * sizeof(int));
    size_t error_count = 0;
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
                index -= 1;
                (*error_positions)[error_count++] = i + index / 8;
            } else {
                free(*error_positions);
                return 1;
            }
        }
    }

    *error_positions = realloc(*error_positions, error_count * sizeof(int));
    return error_count;
}

void show_files_in_console_protected(char *original_content, size_t size, char *compressed_content, size_t compressed_size, int block_size) {
    int *original_errors = NULL;
    int *compressed_errors = NULL;

    int original_error_count = correct_error_and_get_positions(original_content, block_size, size, &original_errors);
    int compressed_error_count = correct_error_and_get_positions(compressed_content, block_size, compressed_size, &compressed_errors);

    printf(GREEN "Archivo Original (Hexadecimal):                    ");
    printf(RED "Archivo Protegido con errores (Hexadecimal):\n" RESET);

    size_t max_size = size > compressed_size ? size : compressed_size;
    for (size_t i = 0; i < max_size; i += 16) {
        for (size_t j = i; j < i + 16 && j < size; j++) {
            int is_error = 0;
            for (int k = 0; k < compressed_error_count; k++) {
                if (compressed_errors[k] == j) {
                    is_error = 1;
                    break;
                }
            }
            if (is_error) {
                printf(GREEN "%02x " RESET, (unsigned char)original_content[j]);
            } else {
                printf("%02x ", (unsigned char)original_content[j]);
            }
        }
        if (i + 16 > size) {
            for (size_t j = 0; j < (i + 16 - size); j++) {
                printf("   ");
            }
        }
        printf("    ");
        for (size_t j = i; j < i + 16 && j < compressed_size; j++) {
            int is_error = 0;
            for (int k = 0; k < compressed_error_count; k++) {
                if (compressed_errors[k] == j) {
                    is_error = 1;
                    break;
                }
            }
            if (is_error) {
                printf(RED "%02x " RESET, (unsigned char)compressed_content[j]);
            } else {
                printf("%02x ", (unsigned char)compressed_content[j]);
            }
        }
        printf("\n");
    }

    free(original_errors);
    free(compressed_errors);
}