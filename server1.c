#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")  // Link with the Winsock library

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

const char *get_mime_type(const char *file_path) {

    
    if (strstr(file_path, ".html")) return "text/html";
    if (strstr(file_path, ".css")) return "text/css";
    if (strstr(file_path, ".js")) return "application/javascript";
    if (strstr(file_path, ".jpg")) return "image/jpeg";
    if (strstr(file_path, ".png")) return "image/png";
    if (strstr(file_path, ".gif")) return "image/gif";
    if (strstr(file_path, ".svg")) return "image/svg+xml";
    if (strstr(file_path, ".ico")) return "image/x-icon";
    if (strstr(file_path, ".mp4")) return "video/mp4";
    if (strstr(file_path, ".mp3")) return "audio/mpeg";
    if (strstr(file_path, ".pdf")) return "application/pdf";

    return "text/plain";  
}

// Function to handle client requests in a separate thread
DWORD WINAPI handle_client_request(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    char request_buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        printf("Error: Failed to receive data from client.\n");
        closesocket(client_socket);
        return 1;
    }

    request_buffer[bytes_received] = '\0';
    printf("Received Request:\n%s\n", request_buffer);

    // Extract the requested file path from the HTTP request
    char requested_file_path[256] = "index.html";  // Default file to serve
    char *request_line = strtok(request_buffer, "\r\n");
    if (request_line) {
        char http_method[10], requested_url[256];
        sscanf(request_line, "%s %s", http_method, requested_url);

        // Ensure the request is a GET request
        if (strcmp(http_method, "GET") != 0) {
            printf("Error: Only GET requests are supported.\n");
            closesocket(client_socket);
            return 1;
        }

        // Remove the leading "/" from the URL and set the file path
        if (strcmp(requested_url, "/") != 0) {
            snprintf(requested_file_path, sizeof(requested_file_path), "%s", requested_url + 1);
        }
    }

    // Attempt to open the requested file
    FILE *requested_file = fopen(requested_file_path, "rb");
    if (!requested_file) {
        // Serve a 404 error page if the file is not found
        const char *not_found_response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html>"
            "<body><h1>404 - Page Not Found</h1>"
            "<p>The requested URL was not found on this server.</p></body></html>";

        send(client_socket, not_found_response, strlen(not_found_response), 0);
    } else {
        // Determine the file size
        fseek(requested_file, 0, SEEK_END);
        long file_size = ftell(requested_file);
        rewind(requested_file);

        // Read the file content into memory
        char *file_content = malloc(file_size);
        fread(file_content, 1, file_size, requested_file);
        fclose(requested_file);

        // Determine the MIME type based on the file extension
        const char *mime_type = get_mime_type(requested_file_path);

        // Send the HTTP response header
        char response_header[BUFFER_SIZE];
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Length: %ld\r\n"
                 "Content-Type: %s\r\n"
                 "\r\n",
                 file_size, mime_type);

        send(client_socket, response_header, strlen(response_header), 0);
        send(client_socket, file_content, file_size, 0);

        free(file_content);
    }

    closesocket(client_socket);
    return 0;
}

int main() {
    WSADATA winsock_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int client_address_length = sizeof(client_address);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &winsock_data) != 0) {
        printf("Error: Failed to initialize Winsock.\n");
        return 1;
    }

    // Create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Error: Failed to create server socket.\n");
        WSACleanup();
        return 1;
    }

    // Bind the server socket to the specified port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Error: Failed to bind server socket.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Error: Failed to start listening on server socket.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is running on http://localhost:%d\n", SERVER_PORT);

    // Main server loop to accept and handle client connections
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == INVALID_SOCKET) {
            printf("Error: Failed to accept client connection.\n");
            continue;
        }

        // Create a new thread to handle the client request
        HANDLE client_thread = CreateThread(NULL, 0, handle_client_request, &client_socket, 0, NULL);
        if (client_thread == NULL) {
            printf("Error: Failed to create client thread.\n");
            closesocket(client_socket);
        } else {
            CloseHandle(client_thread);  // Close the thread handle as we don't need it
        }
    }

    // Cleanup and close the server socket
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

