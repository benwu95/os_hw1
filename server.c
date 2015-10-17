#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/* Read text from the socket and print it out. Continue until the
 * socket closes. Return nonzero if the client sent a “quit”
 * message, zero otherwise. */

int server (int clientSocket) {
    while (1) {
        int length;
        char* text;
        
        /* First, read the length of the text message from the socket. If
         * read returns zero, the client closed the connection. */
        if (read(clientSocket, &length, sizeof(length)) == 0)
            return 0;
        /* Allocate a buffer to hold the text. */
        text = (char*) malloc (length);
        /* Read the text itself, and print it. */
        read(clientSocket, text, length);
        printf("%s\n", text);
        /* Free the buffer. */
        free(text);
        /* If the client sent the message "quit"
         * we're all done.*/
        if (!strcmp(text, "quit"))
            return 1;
    }
}

int main (int argc, char* const argv[]) {
    const char* const socketName = argv[1];
    int socketFd;
    struct sockaddr_un name;
    int clientSentQuitMessage;

    /* Create the socket. */
    socketFd = socket(PF_LOCAL, SOCK_STREAM, 0);
    /* Indicate that is a server. */
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socketName);
    bind(socketFd, &name, SUN_LEN(&name));
    /* Listen for connections. */
    listen(socketFd, 5);
    /* Repeatedly accept connections, spinning off one server() to deal
     * with each client. Continue until a client sends a “quit” message. */
    do {
        struct sockaddr_un clientName;
        socklen_t clientNameLen;
        int clientSocketFd;

        /* Accept a connection. */
        clientSocketFd = accept(socketFd, &clientName, &clientNameLen);
        /* Handle the connection. */
        slientSentQuitMessage = server(clientSocketFd);
        /* Close our end of the connection. */
        close(clientSocketFd);
    }
    while (!ClientSentQuitMessage);

    /* Remove the socket file. */
    close(socketFd);
    unlink(socketName);

    return 0;
}
