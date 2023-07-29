#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

void handle_client(int client_socket) {
    char buffer[5];

    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                cout << "Client disconnected" << endl;
            } else {
                perror("Error receiving data");
            }
            break;
        };

        buffer[bytes_received] = '\0';
        cout << "Received from client: " << buffer << endl;

        int bytes_sent = send(client_socket, buffer, bytes_received, 0);
        if (bytes_sent < 0) {
            perror("Error sending data to client");
            break;
        };
    };
}

int start_server() {
    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 3) {
        perror("Error creating socket");
        exit(1);
    };

    // Create address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(0);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error in binding socket");
        exit(1);    
    };

    // Listen
    if (listen(server_socket, 5) < 0) {
        perror("Error listening on socket");
        exit(1);
    };

    // Retrieve the assigned port number
    socklen_t addr_len = sizeof(server_address);
    getsockname(server_socket, (struct sockaddr *)&server_address, &addr_len);
    int assigned_port = ntohs(server_address.sin_port);
    printf("Server listening on port: %d\n", assigned_port);

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);

        // Accept
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Error accepting client connection");
            continue;
        };
        cout << "Received connection from: " << inet_ntoa(client_address.sin_addr) << endl;
        
        // Fork
        int child_pid = fork();
        if (child_pid < 0) {
            perror("Error forking process");
            close(client_socket);
            continue;
        };

        if (child_pid == 0) {
            handle_client(client_socket);
            exit(0);
        } else {
            cout << "Closing client socket from parent side\n" << endl; 
            close(client_socket);
        };
    };

    cout << "Closing server socket " << server_socket << endl; 
    close(server_socket);
};

int main() {
    start_server();
    return 0;
};