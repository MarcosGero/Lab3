#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "xhamming.h"


#define MAX_FILENAME 260
#define MAX_CONTENT_SIZE 8192

// ANSI escape codes para colores
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

// Función para mostrar contenido en formato hexadecimal
void print_hex(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}
// Función para calcular estadísticas de compresión
void calculate_compression_statistics(size_t original_size, size_t compressed_size) {
    double compression_ratio = (double)compressed_size / original_size * 100;
    printf("Tamaño original: %zu bytes\n", original_size);
    printf("Tamaño comprimido: %zu bytes\n", compressed_size);
    printf("Ratio de compresión: %.2f%%\n", compression_ratio);
}

// Función para calcular estadísticas de protección
void calculate_protection_statistics(size_t original_size, size_t protected_size) {
    double overhead = (double)protected_size / original_size * 100;
    printf("Tamaño original: %zu bytes\n", original_size);
    printf("Tamaño protegido: %zu bytes\n", protected_size);
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


// Función principal
int main() {
    char input_filename[MAX_FILENAME];
    char input_filename_noExtension[MAX_FILENAME];
    char output_filename[MAX_FILENAME];
    int choice;
    int block_size; // Para hamming

    printf("Laboratorio 3 [Huffman y Hamming]\n");
    printf("Gelves Marcos, Julian Jeremias A.\n\n");


    while (1) {
        // Huffman
        printf("[HUFFMAN]\n");
        printf("1.  Compactar archivo\n");
        printf("2.  Descompactar archivo\n");

        // Hamming
        printf("[HAMMING]\n");
        printf("3.  Proteger archivo\n");
        printf("4.  Introducir errores (por modulo)\n");
        printf("5.  Introducir dos errores (por modulo)\n");
        printf("6.  Decodificar archivo con errores\n");
        printf("7.  Decodificar archivo corrigiendo errores\n");

        // Analiticas
        printf("[ESTADISTICAS]\n");
        printf("8.   Mostrar contenido archivo\n");
        printf("9.   Comparar tama%cos (Hexadecimal)\n",164);
        printf("10.  Comparar tama%cos (Binario)\n",164);
        printf("11.  Comparar tama%cos (Caracteres)\n\n",164);
        printf("12.  Estadísticas de Compresión\n");
        printf("13.  Estadísticas de Protección\n");
        printf("0.  Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &choice);

        switch (choice) {
            // HUFFMAN
            case 1:
                printf("Ingrese el nombre del archivo a comprimir: \n");
                scanf("%s", input_filename);
                strcpy(input_filename_noExtension, input_filename);
                input_filename_noExtension[strlen(input_filename) - 4] = '\0';
                sprintf(output_filename, "%s.huf", input_filename_noExtension);

                if (comprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo comprimido creado como: %s\n", output_filename);

                break;
            case 2:
                printf("Ingrese el nombre del archivo a descomprimir: \n");
                scanf("%s", input_filename);
                strcpy(input_filename_noExtension, input_filename);
                input_filename_noExtension[strlen(input_filename) - 4] = '\0';
                sprintf(output_filename, "%s-decompressed.txt", input_filename_noExtension);

                if (descomprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo descomprimido creado como: %s\n", output_filename);

                break;

            // HAMMING
            case 3:
                printf("Ingrese el nombre del archivo a proteger: ");
                scanf("%s", input_filename);
                printf("Ingrese el tama%co de bloque\n1: 8 bits\n2: 4096 bits\n3: 65536 bits\n",164);
                scanf("%d", &block_size);

                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';


                sprintf(output_filename, "%s.HA%d", input_filename_noExtension, block_size);

                if(protect_file(input_filename, getBlockSizeByIndex(block_size), output_filename) == 0)
                    printf("Archivo protegido creado como: %s\n", output_filename);
                break;
            case 4:
                printf("Ingrese el nombre del archivo a introducir errores: ");
                scanf("%s", input_filename);

                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';


                sprintf(output_filename, "%s.HE", input_filename_noExtension);
                introduce_errors(input_filename, output_filename);
                printf("Archivo con errores creado como: %s\n", output_filename);
                break;
            case 5:
                printf("Ingrese el nombre del archivo a introducir los dos errores: ");
                scanf("%s", input_filename);

                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';


                sprintf(output_filename, "%s.HE", input_filename_noExtension);
                introduce_two_errors(input_filename, output_filename);
                printf("Archivo con errores creado como: %s\n", output_filename);
                break;
            case 6:
                printf("Ingrese el nombre del archivo a decodificar sin corregir errores: ");
                scanf("%s", input_filename);
                //printf("Ingrese el tamaño de bloque (8, 4096, 65536 bits): ");
                //scanf("%d", &block_size);
                block_size = getBlockSizeByExtension(input_filename);
                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';

                sprintf(output_filename, "%s.DE%d", input_filename_noExtension,getIndexForExtension(block_size));

                decode_file(input_filename, output_filename, block_size, 0);
                printf("Archivo decodificado sin corrección creado como: %s\n", output_filename);
                break;
            case 7:
                printf("Ingrese el nombre del archivo a decodificar corrigiendo errores: ");
                scanf("%s", input_filename);

                block_size = getBlockSizeByExtension(input_filename);
                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';
                sprintf(output_filename, "%s.DEC", input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';

                sprintf(output_filename, "%s.DC%d", input_filename_noExtension,getIndexForExtension(block_size));

                decode_file(input_filename, output_filename, block_size, 1);
                printf("Archivo decodificado con corrección creado como: %s\n", output_filename);
                break;

            // ESTADISTICAS
            // Mostrar contenido archivo
            case 8:
                printf("Ingrese el nombre del archivo: ");
                scanf("%s", input_filename);

                size_t size;
                char *content = load_file(input_filename, &size);
                if (content) {
                    printf("Contenido del archivo:\n%s\n", content);
                    free(content);
                } else {
                    printf("Error al cargar el archivo.\n");
                }
                break;
            case 9:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size;
                char *original_content = load_file(input_filename, &original_size);
                if (!original_content) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo comprimido: ");
                scanf("%s", output_filename);
                size_t compressed_size;
                char *compressed_content = load_file(output_filename, &compressed_size);

                if (!compressed_content) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content);
                    break;
                }

                show_files_in_console_hex(original_content, original_size, compressed_content, compressed_size);

                free(original_content);
                free(compressed_content);
                break;
            case 10:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size_bin;
                char *original_content_bin = load_file(input_filename, &original_size_bin);
                if (!original_content_bin) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo comprimido: ");
                scanf("%s", output_filename);
                size_t compressed_size_bin;
                char *compressed_content_bin = load_file(output_filename, &compressed_size_bin);
                if (!compressed_content_bin) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content_bin);
                    break;
                }

                show_files_in_console_binary(original_content_bin, original_size_bin, compressed_content_bin, compressed_size_bin);

                free(original_content_bin);
                free(compressed_content_bin);
                break;
            case 11:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size_char;
                char *original_content_char = load_file(input_filename, &original_size_char);
                if (!original_content_char) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo comprimido: ");
                scanf("%s", output_filename);
                size_t compressed_size_char;
                char *compressed_content_char = load_file(output_filename, &compressed_size_char);
                if (!compressed_content_char) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content_char);
                    break;
                }

                show_files_in_console_chars(original_content_char, original_size_char, compressed_content_char, compressed_size_char);

                free(original_content_char);
                free(compressed_content_char);
                break;
            case 12:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size_comp;
                char *original_content_comp = load_file(input_filename, &original_size_comp);
                if (!original_content_comp) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                change_extension_to_huf(input_filename, output_filename);
                size_t compressed_size_comp;
                char *compressed_content_comp = load_file(output_filename, &compressed_size_comp);
                if (!compressed_content_comp) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content_comp);
                    break;
                }

                calculate_compression_statistics(original_size_comp, compressed_size_comp);

                free(original_content_comp);
                free(compressed_content_comp);
                break;
            case 13:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size_prot;
                char *original_content_prot = load_file(input_filename, &original_size_prot);
                if (!original_content_prot) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo protegido: ");
                scanf("%s", output_filename);
                size_t protected_size;
                char *protected_content = load_file(output_filename, &protected_size);
                if (!protected_content) {
                    printf("Error al cargar el archivo protegido.\n");
                    free(original_content_prot);
                    break;
                }

                calculate_protection_statistics(original_size_prot, protected_size);

                free(original_content_prot);
                free(protected_content);
                break;
            case 0:
                return 0;
            default:
                printf("Opción inválida.\n");
        }
        wait_for_enter();
    }
}

