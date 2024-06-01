#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#define MAX_FILENAME 256
void char_to_binary(char c, char *output) {
    for (int i = 7; i >= 0; i--) {
        output[7 - i] = ((c >> i) & 1) ? '1' : '0';
    }
    output[8] = '\0';
}

void inspect_pointer(char *ptr) {
    char binary[9];
    char_to_binary(*ptr, binary);
    printf("Value at pointer: %s\n", binary);
}
// Devuelve la cantidad de bit de control que deberia tener un bloque de hamming
int get_parity_bit_count(int n) {
    int k = 0;
    while ((1 << k) < n + k + 1) {
        k++;
    }
    return k;
}

// Encuentra y corrige errores en un bloque usando Hamming
int find_and_correct_errors(char *block, int size) {
    int syndrome = 0;
    int bit_position = 0;

    // First pass: Determine the syndrome
    for (int i = 0; i < size; i++) {
        char c = block[i];
        for (int bit = 0; bit < 8; bit++) {
            if (c & (1 << bit)) {
                syndrome ^= bit_position;
            }
            bit_position++;
        }
    }

    // If syndrome is not zero, correct the error at the syndrome bit position
    if (syndrome != 0) {
        int char_index = syndrome / 8;  // Find the character index in the block
        int bit_index = syndrome % 8;   // Find the bit index in the character

        if (char_index < size) {
            block[char_index] ^= (1 << bit_index);  // Flip the erroneous bit
            return syndrome;  // Return the corrected position
        }
        return -1;  // Error position out of bounds, indicating multiple errors or other issue
    }
    return 0;  // No error found
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

// Función para proteger el archivo usando el código de Hamming
int protect_file(const char *input_filename, int data_size, const char *output_filename) {
    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) return 1;

    int total_bits = size * 8;
    int k = get_parity_bit_count(data_size);
    int n = data_size;
    int block_size = n + k + 1;
    printf("n, k: (%i, %i), blocksize: %i \n",n,k,block_size);

    // Preparar archivo de Salida
    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error creating protected file");
        free(data);
        return 2;
    }

    // Pide memoria para un unico bloque
    int block_bytes = (block_size + 7) / 8;
    printf("Block Size in Bytes: %d: \n",block_bytes);
    char *block = malloc(block_bytes); // Pide espacio para un unico bloque que se reutilizará por cada iteracion de hamming
    if (!block) {
        perror("Memory allocation failed for block");
        fclose(out);
        free(data);
        return 3;
    }



    // Aplicar Hamming a cada bloque
    // Esto itera por cada cantidad
    // de bits de info tenga el bloque de datos
    // sin codificar, en cada iteracion
    // se acomodan los bits de info en un nuevo bloque
    // (de modo que los bits de control queden en cero), y se calculan los bits de hamming
    for (int i = 0; i < total_bits; i += n) {
        memset(block, 0, block_bytes); // Limpiar el bloque antes de usarlo


        int bitPosACopiar = 0;

        for(int j = 0; j < n+k; j++){ // recorre all the bloque

            if (bitPosACopiar>size*8) break;


            if (!isPowerOfTwo(j+1)){
                set_bit(block,j,get_bit(data,i+bitPosACopiar));
                char *chartest = &data[i];
                inspect_pointer(chartest);
                bitPosACopiar++;
            }

        }
        inspect_pointer(block);
        calculate_parity_bits(block, block_size, k);
        inspect_pointer(block);
        calculate_block_parity(block,block_size);
        inspect_pointer(block);

        fwrite(block, 1, block_bytes, out);  // Escribir bloque protegido
        printf("<Procesado bloque %d>\n",i/n);
    }

    fclose(out);
    free(block);
    free(data);
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

// Introduce errores aleatorios en el archivo
int introduce_errors(const char *input_filename, const char *output_filename) {
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
    sprintf(indexString, "%d", getIndexForExtension(block_size));
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
        char* chartest= &data[i];
        inspect_pointer(chartest);
        int errorPosition = rand() % block_size;
//        printf("%d %d\n",errorPosition,block_size);
//        printf("Index iter: %d\n",i);
//        printBlock(data,block_size);
        set_bit(data,i+errorPosition,!get_bit(data,i+errorPosition));
//        printBlock(data,block_size);
        chartest= &data[i];
        inspect_pointer(chartest);
    }

    fwrite(data, 1, size, out);
    fclose(out);
    free(data);
    return 0;
}


int correct_error(const char *data,  int block_size, size_t size){


        int block_bytes = (block_size-1 + 7) / 8;

        for (int i = 0; i < size; i += block_bytes) {
            int index = 0;
            int parity = 0;
            for (int j = 0; j < block_size; j++) {

                if(get_bit(data,i*8+j)){
                    index ^= (i*8+j)+1;
                    parity ^= 1;
                }
            }

            if(index){
                if (parity % 2 != 0) {
                    // Paridad impar, un solo error
                    index -= 1;
                    printf("Error encontrado,corrigiendo...\n");
                    set_bit(data, i + index, !get_bit(data, i + index));
                }else {
                    printf("Se encontraron dos errores en el bloque a partir de la posición %d\n", i);
                    return 1; // Retornar indicando que hubo un error que no se pudo corregir
                }
            }else{
                printf("No se encontraron errores c:\n");
            }
        }
        return 0;

}

// Función para decodificar el archivo, opcionalmente corrigiendo errores
int decode_file(const char *input_filename, const char *output_filename, int block_size, int correct_errors) {
    size_t size;
    char *data = load_file(input_filename, &size);
    if (!data) return 1;

    int parity_bits = get_parity_bit_count(block_size); // nro bits de control
    int data_bits = block_size; // nro bits de info
    block_size = data_bits + parity_bits+1;
    int data_bytes = data_bits/8 + (data_bits % 8 != 0); // tecnica de redondeo hacia arriba

    int block_bytes = (block_size + 7) / 8;

    int numberOfBlocks = size/block_bytes;
    int totalDataBits = numberOfBlocks*data_bits;
    int totalDataBytes = totalDataBits/8;


//    block_size = getBlockSizeByExtension(input_filename);

    printf("%d,%d,%d,%d,%d, %d, %d\n",parity_bits,data_bits,data_bytes,numberOfBlocks,block_size,totalDataBytes,totalDataBits);


    // Pide espacio para la cadena de caracteres decodificada
    char *output_data = malloc(size);
    memset(output_data,0,size);

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado");
        free(data);
        return 2;
    }

    if(correct_errors){
        correct_error(data,block_size,size);
    }

    int output_index = 0;
    for (int i = 0; i < size; i += block_bytes) {
        char stop = 'a';
        for (int j = 0; j < block_size; j++) {

            // Copia los bits de data en output_data eliminando los bits de control
            if (!isPowerOfTwo(j+1)) {
                int bitToCopy = get_bit(data, i*8+j);
                set_bit(output_data, output_index, bitToCopy);

                // Ir construyendo caracter de control
                set_bit(&stop,output_index%8,bitToCopy);
                // Condicion de parada al leer un caracter 0 (caso en donde el bloque no se usa en su totalidad) (se comprueba cada 8 bits para asegurarse que el caracter haya sido creado en su totalidad)
                if (output_index % 8 == 0 && stop == 0) {
                        output_index--; // Correccion del indice de salida por como funciona el operador modulo
                        printf("NO HAY MAS CARACTERES PARA DECODIFICAR\n");
                        break;
                }
                output_index++;
            }


        }
    }

    // Escribir los datos decodificados al archivo, considerando los bits efectivamente utilizados
    fwrite(output_data, 1, output_index / 8, out); // Write the actual bytes used
    fclose(out);
    free(data);
    free(output_data);
    return 0;
}

// Introducir dos errores

// Introduce errores aleatorios en el archivo
int introduce_two_errors(const char *input_filename, const char *output_filename) {
    size_t size;
    char *data = load_file(input_filename, &size);
    if (!data) return 1;

    // Calcula el tamaño del bloque en base a la extension
    int block_size = getBlockSizeByExtension(input_filename);
    printf("size in bytes: %d, block_size: %d\n",size,block_size);

    // Calcular extension archivo de salida
    char indexString[5];
    sprintf(indexString, "%d", getIndexForExtension(block_size));
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
