#include "server.h"

int main() {
    Server server;
    
    init_server(&server);
    start_server(&server);
    closesocket(server.server_socket);
    WSACleanup();
    return 0;
}
