#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "huffman.h"
#include "xhamming.h"
#include "utilidades.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_FILENAME 260
#define PORT 8888
#define BUFFER_SIZE 1024

void handle_client_request(SOCKET client_socket);
void send_http_response(SOCKET client_socket, const char *response_body);

int main() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);

    setlocale(LC_ALL, "");

    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Error creating socket\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is running on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            continue;
        }

        handle_client_request(client_socket);
        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

void handle_client_request(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    char input_filename[MAX_FILENAME];
    char input_filename_noExtension[MAX_FILENAME];
    char output_filename[MAX_FILENAME];
    char response[BUFFER_SIZE];

    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == SOCKET_ERROR || bytes_received == 0) {
        printf("Receive failed or connection closed\n");
        return;
    }

    buffer[bytes_received] = '\0';

    if (strstr(buffer, "POST /huffman/compress") != NULL) {
        sscanf(buffer, "POST /huffman/compress?inputFilename=%s", input_filename);
        strcpy(input_filename_noExtension, input_filename);
        input_filename_noExtension[strlen(input_filename) - 4] = '\0';
        sprintf(output_filename, "%s.huf", input_filename_noExtension);

        if (comprimir_huffman(input_filename, output_filename) == 0) {
            snprintf(response, BUFFER_SIZE, "Archivo comprimido creado como: %s\n", output_filename);
        } else {
            snprintf(response, BUFFER_SIZE, "Error al comprimir el archivo\n");
        }

        send_http_response(client_socket, response);
    } else {
        send_http_response(client_socket, "Error: Invalid endpoint or method\n");
    }
}

void send_http_response(SOCKET client_socket, const char *response_body) {
    char response[BUFFER_SIZE];
    int response_length = snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: http://localhost:3000\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n"
        "%s", response_body);

    send(client_socket, response, response_length, 0);
}
