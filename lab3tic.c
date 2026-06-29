#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "huffman.h"
#include "xhamming.h"
#include "utilidades.h"


#define MAX_FILENAME 260
#define MAX_CONTENT_SIZE 8192




// Función principal
int main() {
    setlocale(LC_ALL, "");
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
        printf("9.   Comparar archivos protegidos\n");
        printf("10.  Comparar tamaños (Hexadecimal)\n");
        printf("11.  Comparar tamaños (Binario)\n");
        printf("12.  Comparar tamaños (Caracteres)\n\n");
        printf("13.  Estadisticas de Compresión\n");
        printf("14.  Estadisticas de Protección\n");
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
            scanf("%255s", input_filename);

            size_t size;
            char *content = load_file(input_filename, &size);
            if (content) {
                printf("Contenido del archivo (tamaño: %zu):\n", size);
                printf("%s\n", content);
                free(content);
            } else {
                printf("Error al cargar el archivo.\n");
            }
            break;
            case 9: {
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size;
                char *original_content = load_file(input_filename, &original_size);
                if (!original_content) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo protegido con errores a comparar: ");
                scanf("%s", output_filename);
                size_t compressed_size;
                char *compressed_content = load_file(output_filename, &compressed_size);

                if (!compressed_content) {
                    printf("Error al cargar el archivo protegido .\n");
                    free(original_content);
                    break;
                }
                block_size = getBlockSizeByExtension(input_filename);
                show_files_in_console_protected(original_content, original_size, compressed_content, compressed_size,block_size);

                free(original_content);
                free(compressed_content);
                break;
            }
            case 10:{
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
             }
            case 11:
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
            case 12:
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
            case 13:
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
            case 14:
                printf("Ingrese el nombre del archivo original: ");
            scanf("%s", input_filename);
            size_t original_size_prot;
            char *original_content_prot = load_file(input_filename, &original_size_prot);
            if (!original_content_prot) {
                printf("Error al cargar el archivo original.\n");
                break;
            }

            char output_filename1[256];
            char output_filename2[256];
            char output_filename3[256];
            change_extension_to_ha(input_filename, output_filename1, output_filename2, output_filename3);

            size_t protected_size1, protected_size2, protected_size3;
            char *protected_content1 = load_file(output_filename1, &protected_size1);
            char *protected_content2 = load_file(output_filename2, &protected_size2);
            char *protected_content3 = load_file(output_filename3, &protected_size3);

            if (!protected_content1 || !protected_content2 || !protected_content3) {
                printf("Error al cargar los archivos protegidos.\n");
                free(original_content_prot);
                if (protected_content1) free(protected_content1);
                if (protected_content2) free(protected_content2);
                if (protected_content3) free(protected_content3);
                break;
            }

            // Calcular las estadísticas de protección
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
            printf("Proteccion con Bloques de 8 bits.\n");
            printf("------------------------------------.\n");
            calculate_protection_statistics(original_size_prot, protected_size1);
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
            printf("Proteccion con Bloques de 4096 bits.\n");
            printf("------------------------------------.\n");
            calculate_protection_statistics(original_size_prot, protected_size2);
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.\n");
            printf("Proteccion con Bloques de 65536 bits.\n");
            printf("------------------------------------.\n");
            calculate_protection_statistics(original_size_prot, protected_size3);

            free(original_content_prot);
            free(protected_content1);
            free(protected_content2);
            free(protected_content3);
            break;
            case 0:
                return 0;
            default:
                printf("Opción inválida.\n");
        }
        wait_for_enter();
    }
}

