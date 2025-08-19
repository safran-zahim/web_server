#include "server.h"

void send_404_response(SOCKET client_socket) {
    const char *not_found_response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body><h1>404 - Not Found</h1></body></html>";

    send(client_socket, not_found_response, strlen(not_found_response), 0);
}

void send_500_response(SOCKET client_socket) {
    const char *internal_error_response =
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body><h1>500 - Internal Server Error</h1></body></html>";

    send(client_socket, internal_error_response, strlen(internal_error_response), 0);
}

// Get content type of a file
const char *contenttype(const char *path) {

 
    if (strstr(path, ".gif")) return "image/gif";
    if (strstr(path, ".ico")) return "image/x-icon";
    if (strstr(path, ".xml")) return "application/xml";
    if (strstr(path, ".pdf")) return "application/pdf";
    if (strstr(path, ".zip")) return "application/zip";
    if (strstr(path, ".json")) return "application/json";
    if (strstr(path, ".html")) return "text/html";
    if (strstr(path, ".css")) return "text/css";
    if (strstr(path, ".js")) return "application/javascript";
    if (strstr(path, ".jpg")) return "image/jpeg";
    if (strstr(path, ".png")) return "image/png";

    return "text/plain";
}

// Handle client requests
DWORD WINAPI client_request(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    char request_buffer[BUFFER_SIZE];

    int bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        printf("Failed to receive data\n");
        send_500_response(client_socket);

        closesocket(client_socket);
        return 1;
    }

    request_buffer[bytes_received] = '\0';
    printf("Received Request:\n%s\n", request_buffer);

    //file path
    char requested_file_path[256] = "index.html";  // Default file location
    char *request_line = strtok(request_buffer, "\r\n");

    if (request_line) {

        char http_method[10], requested_url[256];
        sscanf(request_line, "%s %s", http_method, requested_url);
    
    }

    // Open file
    FILE *requested_file = fopen(requested_file_path, "rb");

    if (!requested_file) {
        printf("File %s not found.\n", requested_file_path);
        send_404_response(client_socket);

    } else {

        //  file size cal
        fseek(requested_file, 0, SEEK_END);
        long file_size = ftell(requested_file);
        rewind(requested_file);

        // Read content
        char *file_content = malloc(file_size);
        
        if (!file_content) {

            send_500_response(client_socket);
            fclose(requested_file);

            closesocket(client_socket);

            return 1;

        }

        fread(file_content, 1, file_size, requested_file);

        fclose(requested_file);

        // file type
        const char *mime_type = contenttype(requested_file_path);

        // Send response h
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
