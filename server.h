#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

typedef struct {
    WSADATA winsock_data;
    SOCKET server_socket;
    struct sockaddr_in server_address;
} Server;

void handle_error(const char *message);
void init_server(Server *server);
void start_server(Server *server);
DWORD WINAPI client_request(LPVOID client_socket_ptr);
void send_404_response(SOCKET client_socket);
void send_500_response(SOCKET client_socket);
const char *contenttype(const char *path);

