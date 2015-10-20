#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>

#define BUF 10000

int createServer(int &socketFd, struct sockaddr_in &server, int port) {
    /* Create a socket. */
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        puts("Could not create socket!");
        return 1;
    }
    /* Set the socket. */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    /* Bind socket to a port. */
    if (bind(socketFd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        puts("Bind failed!");
        return 1;
    }
    return 0;
}

int createFile(char* fileName) {
    FILE* fp;
    struct stat fileStat;
    /* Check file. */
    if (stat(fileName, &fileStat) < 0)
        return 1;
    /* Create file. */
    fp = fopen(fileName, "w");
    fclose(fp);
    return 0;
}

int deleteFile(char* fileName) {
    FILE* fp;
    struct stat fileStat;
    /* Check file. */
    if (stat(fileName, &fileStat) < 0)
        return 1;
    /* Delete file. */
    if (remove(fileName) != 0)
        perror("Delete failed!");
    return 0;
}

void listFile() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL)
            printf("%s\n", dir->d_name);
        closedir(d);
    }
}

int editFile(char* fileName, int tmpSocket) {
    FILE* fp;
    struct stat fileStat;
    char* buf = (char*) malloc (sizeof(char) * BUF);
    int bytes;
    /* Check file. */
    if (stat(fileName, &fileStat) < 0)
        return 1;
    fp = fopen(fileName, "a");
    
    fclose(fp);
    free(buf);
    return 0;
}

int DownloadFile(char* fileName, int tmpSocket) {
    FILE* fp;
    struct stat fileStat;
    char* buf = (char*) malloc (sizeof(char) * BUF);
    int bytes;
    /* Check file. */
    if (stat(fileName, &fileStat) < 0)
        return 1;
    printf("File Size: \t%d bytes\n", fileStat.st_size);
    fp = fopen(fileName, "rb");
    /* Send file. */
    while (!feof(fp)) {
        bytes = fread(buf, sizeof(char), sizeof(buf), fp);
        printf("fread %d bytes, ", bytes);
        bytes = write(tmpSocket, buf, bytes);
        printf("Sending %d bytes\n", bytes);
    }
    fclose(fp);
    free(buf);
    return 0;
}

int main(int argc, char* argv[]) {
    int socketFd;
    struct sockaddr_in server;
    int port = atoi(argv[1]);

    /* Start the server. */
    if (createServer(socketFd, server, port)) {
        return 0;
    } else {
        /* Listen. */
        listen(socketFD, 5);
        while (1) {
            int clientSocket;
            struct sockaddr_in client;
            socklen_t length = sizeof(clientSocket);
            int tmpSocket = accept(socketFd, (struct sockaddr*)&client,
                                    &length);
            if (tmpSocket < 0) {
                puts("Server accept failed!");
                break;
            }
            
            
            
            close(tmpSocket);
        }
    }
    close(socketFd);
    return 0;
}
