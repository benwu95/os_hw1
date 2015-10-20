#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    char* socketName = argv[1];
    int port = atoi(argv[2]);
    int socketFd;
    struct sockaddr_in name;

    /* Create the socket. */
    socketFd = socket(PF_LOCAL, SOCK_STREAM, 0);
    /* Store the server's name in the socket address. */
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socketName);
    /* Connect the socket. */
    connect(socketFd, &name, SUN_LEN(&name));
    
    close(socketFd);
    return 0;
}
