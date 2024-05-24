#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_FILENAME 256

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
        } else {
            return -1;  // Error position out of bounds, indicating multiple errors or other issues
        }


        set_bit(block,syndrome,!get_bit(block,syndrome));
    }



    return 0;  // No error found
}

// Calcula los bits de paridad y configura en el array 'data'
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

// Función para proteger el archivo usando el código de Hamming
int protect_file(const char *input_filename, int block_size, const char *output_filename) {
    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) return 1;

    int total_bits = size * 8;
    int k = get_parity_bit_count(block_size);
    int n = block_size - k;
    printf("n, k: (%i, %i)\n",n,k);

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
    for (int i = 0; i < total_bits; i += n) { // Esto itera por cada cantidad de bits de info tenga el bloque de datos sin codificar, en cada iteracion se acomodan los bits de info en un nuevo bloque (de modo que los bits de control queden en cero), y se calculan los bits de hamming
        memset(block, 0, block_bytes); // Limpiar el bloque antes de usarlo

//        printBlock(data,size);
//        printf("Antes copiado\n");
//        printBlock(block,block_bytes);

        int bitPosACopiar = 0;
        for(int j = 0; j < n+k; j++){ // recorre todo el bloque
            // en i+j estaria el bit que nos interesa copiar en la nueva posicion
            // solo las posiciones de j que no sean potencias de dos se copiaran

//            printf("(i: %d,j: %d)\n",i,j);


            // Condicion parada en caso de que el bloque sea mas grande que el archivo a copiar (para evitar datos basura)
            if (bitPosACopiar>size*8) break;
//            printf("%d, exceso del archivo %d\n",bitPosACopiar,size*8);



            if (!isPowerOfTwo(j+1)){
//                printf("no es potencia\n");
//                printf("Bit a copiar: %d en posicion: %d\n",get_bit(data,i+bitPosACopiar),j);
                set_bit(block,j,get_bit(data,i+bitPosACopiar));
                bitPosACopiar++;
            }

//            printf("%d\n",bitPosACopiar);
        }
//        printf("Despues copiado\n");
//        printBlock(block,block_bytes);

////        int limit = fmin(n, total_bits - i);
////        memset(block, 0, block_bytes); // Limpiar el bloque antes de usarlo

        calculate_parity_bits(block, block_size, k);
//        printf("Despues de aplicar Hamming\n");
//        printBlock(block,block_bytes);

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
int getBlockSizeByExtension(char* input_filename){
    return getBlockSizeByIndex(getExtensionIndex(input_filename));
}

// Dado el tipo de bloque, retorna el tamaño del bloque
int getBlockSizeByIndex(int n){
    switch(n){
        case 1:
            return 8;
            break;
        case 2:
            return 4096;
            break;
        case 3:
            return 65536;
            break;
        default:
            return 8;
    }
}

// Dado el tamaño del bloque, retorna su codificacion para la extension del archivo protegido
int getIndexForExtension(int n){
    switch(n){
        case 8:
            return 1;
            break;
        case 4096:
            return 2;
            break;
        case 65536:
            return 3;
            break;
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
//        printBlock(data,block_size);
    }

    fwrite(data, 1, size, out);
    fclose(out);
    free(data);
    return 0;
}


int correct_error(const char *data,  int block_size, size_t size){

        int parity_bits = get_parity_bit_count(block_size); // nro bits de control
        int data_bits = block_size - parity_bits; // nro bits de info
        int data_bytes = data_bits/8 + (data_bits % 8 != 0); // tecnica de redondeo hacia arriba

        int block_bytes = (block_size + 7) / 8;



        for (int i = 0; i < size; i += block_bytes) {
            int index = 0;
            for (int j = 0; j < block_size; j++) {

                if(get_bit(data,i*8+j)){
                    index ^= (i*8+j)+1;
                }
            }
            if(index){
                index -= 1;
                printf("Error encontrado, corrigiendo...\n");
                if(get_bit(data,index))
                    set_bit(data,index,0);
                else
                    set_bit(data,index,1);
            }else{
                printf("No se encontraron errores c:\n");
            }
        }


}

// Función para decodificar el archivo, opcionalmente corrigiendo errores
int decode_file(const char *input_filename, const char *output_filename, int block_size, int correct_errors) {
    size_t size;
    char *data = load_file(input_filename, &size);
    if (!data) return 1;

    int parity_bits = get_parity_bit_count(block_size); // nro bits de control
    int data_bits = block_size - parity_bits; // nro bits de info
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
//        printBlock(data,size);
//        printf("==========\n");
//        printBlock(output_data,size);
//        printf("==========\n");

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

//        printBlock(output_data,size);
//        printf("<FIN ITERACION: %d>\n",i/block_size);
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


// Menú principal
//int main() {
//    char input_filename[MAX_FILENAME];
//    char input_filename_noExtension[MAX_FILENAME];
//    char output_filename[MAX_FILENAME];
//    int choice;
//    int block_size;
//
//    printf("Programa de manejo de archivos con Códigos de Hamming\n");
//
//    while (1) {
//        printf("\n1. Cargar y mostrar archivo\n");
//        printf("2. Proteger archivo\n");
//        printf("3. Introducir errores\n");
//        printf("4. Decodificar archivo con errores\n");
//        printf("5. Decodificar archivo corrigiendo errores\n");
//        printf("0. Salir\n");
//        printf("Seleccione una opción: ");
//        scanf("%d", &choice);
//
//        switch (choice) {
//            case 1:
//                printf("Ingrese el nombre del archivo: ");
//                scanf("%s", input_filename);
//                size_t size;
//                char *content = load_file(input_filename, &size);
//                if (content) {
//                    printf("Contenido del archivo:\n%s\n", content);
//                    free(content);
//                }
//                break;
//            case 2:
//                printf("Ingrese el nombre del archivo a proteger: ");
//                scanf("%s", input_filename);
//                printf("Ingrese el tamaño de bloque\n1: 8 bits\n2: 4096 bits\n3: 65536 bits\n");
//                scanf("%d", &block_size);
//
//                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
//                strcpy(input_filename_noExtension,input_filename);
//                input_filename_noExtension[strlen(input_filename)-4]='\0';
//
//
//                sprintf(output_filename, "%s.HA%d", input_filename_noExtension, block_size);
//
//                if(protect_file(input_filename, getBlockSizeByIndex(block_size), output_filename) == 0)
//                    printf("Archivo protegido creado como: %s\n", output_filename);
//                break;
//            case 3:
//                printf("Ingrese el nombre del archivo a introducir errores: ");
//                scanf("%s", input_filename);
//
//                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
//                strcpy(input_filename_noExtension,input_filename);
//                input_filename_noExtension[strlen(input_filename)-4]='\0';
//
//
//                sprintf(output_filename, "%s.HE", input_filename_noExtension);
//                introduce_errors(input_filename, output_filename);
//                printf("Archivo con errores creado como: %s\n", output_filename);
//                break;
//            case 4:
//                printf("Ingrese el nombre del archivo a decodificar sin corregir errores: ");
//                scanf("%s", input_filename);
//                //printf("Ingrese el tamaño de bloque (8, 4096, 65536 bits): ");
//                //scanf("%d", &block_size);
//                block_size = getBlockSizeByExtension(input_filename);
//                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
//                strcpy(input_filename_noExtension,input_filename);
//                input_filename_noExtension[strlen(input_filename)-4]='\0';
//
//                sprintf(output_filename, "%s.DE%d", input_filename_noExtension,getIndexForExtension(block_size));
//
//                decode_file(input_filename, output_filename, block_size, 0);
//                printf("Archivo decodificado sin corrección creado como: %s\n", output_filename);
//                break;
//            case 5:
//                printf("Ingrese el nombre del archivo a decodificar corrigiendo errores: ");
//                scanf("%s", input_filename);
//
//                block_size = getBlockSizeByExtension(input_filename);
//                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
//                strcpy(input_filename_noExtension,input_filename);
//                input_filename_noExtension[strlen(input_filename)-4]='\0';
//                sprintf(output_filename, "%s.DEC", input_filename);
//                input_filename_noExtension[strlen(input_filename)-4]='\0';
//
//                sprintf(output_filename, "%s.DEC%d", input_filename_noExtension,getIndexForExtension(block_size));
//
//                decode_file(input_filename, output_filename, block_size, 1);
//                printf("Archivo decodificado con corrección creado como: %s\n", output_filename);
//                break;
//            case 0:
//                return 0;
//            default:
//                printf("Opción inválida.\n");
//        }
//    }
//}
//
//
