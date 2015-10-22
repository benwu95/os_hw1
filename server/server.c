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

#define BUF_MAX 10000
#define FILE_NAME_MAX 256

void createFile(int clientSocketFd, char* fileName) {
    FILE* fp;
    /* Check file. */
    if (fp = fopen(fileName, "r")) {
        fclose(fp);   
        send(clientSocketFd, "File already exists!",
                sizeof("File already exists!"), 0);
    }else {
    /* Create file. */
    fp = fopen(fileName, "w");
    fclose(fp);
    send(clientSocketFd, "Success!", sizeof("Success!"), 0);
    }
}

void removeFile(int clientSocketFd, char* fileName) {
    FILE* fp;
    /* Check file. */
    if (fp = fopen(fileName, "r")) {
        /* Delete file. */
        remove(fileName);
        send(clientSocketFd, "Success!", sizeof("Success!"), 0);
    }else
        send(clientSocketFd, "File doesn't exist!",
                sizeof("File doesn't exist!"), 0);
}

void listFile(int clientSocketFd) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL)
            send(clientSocketFd, dir->d_name, sizeof(dir->d_name), 0);
        closedir(d);
    }
    /* End. */
    send(clientSocketFd, "\\", sizeof("\\"), 0);
}

void editFile(int clientSocketFd, char* fileName) {
    FILE* fp;
    struct stat fileStat;
    char* buf = (char*) malloc (sizeof(char) * BUF_MAX);
    int bytes;
    /* Check file. */
    if (!(fp = fopen(fileName, "r")) ){
        fclose(fp);
        send(clientSocketFd, "File doesn't exist!",
                sizeof("File doesn't exist!"), 0);
    }
    /* Append to the file. */
    fp = fopen(fileName, "a");
    
    fclose(fp);
    free(buf);
    send(clientSocketFd, "Success!", sizeof("Success!"), 0);
}

void downloadFile(int clientSocketFd, char* fileName) {
    FILE* fp;
    struct stat fileStat;
    char* buf = (char*) malloc (sizeof(char) * BUF_MAX);
    char* block = (char*) malloc (sizeof(char)* 2);
    char size[20];
    memset(buf, 0, sizeof(buf));
    int bytes;
    /* Check file. */
    if (fopen(fileName, "r") == NULL) {
        send(clientSocketFd, "NULL", sizeof("NULL"), 0);
        return;
    }
    /* Get the size of the file. */
    if (stat(fileName, &fileStat) == 0) {
        memset(size, 0, sizeof(size));
        sprintf(size, "%ld", fileStat.st_size);
        send(clientSocketFd, size, strlen(size), 0);
    }
    /* Send file. */
    fp = fopen(fileName, "rb");
    while ((bytes = fread(buf, sizeof(char), sizeof(buf), fp)) != 0) {
        memset(size, 0, sizeof(size));
        sprintf(size, "%d", bytes);
        /* Send the buffer bytes. */
        send(clientSocketFd, size, strlen(size), 0);
        /* Block. */
        recv(clientSocketFd, block, sizeof(block), 0);
        /* Send the buffer. */
        send(clientSocketFd, buf, bytes, 0);
        memset(buf, 0, sizeof(buf));
    }
    fclose(fp);
    free(buf);
    free(block);
}

int main(int argc, char* argv[]) {
    int socketFd;
    struct sockaddr_in server;
    char* serverAddr = argv[1];
    int port = atoi(argv[2]);
    char* fileName;

    /* Create a socket. */
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    /* Set the socket. */
    server.sin_addr.s_addr = inet_addr(serverAddr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    /* Bind socket to a port. */
    if (bind(socketFd, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* Listen. */
    if (listen(socketFd, 5)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    /* Start server. */
    int clientSocketFd;
    struct sockaddr_in client;
    socklen_t length = sizeof(clientSocketFd);
    clientSocketFd = accept(socketFd, (struct sockaddr*)&client, &length);
    if (clientSocketFd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    int flag = 1;
    while (flag) {
        /* Get the command. */
        int textLength;
        char command;
        char fileName[FILE_NAME_MAX + 1];
        memset(fileName, 0, sizeof(fileName));
        recv(clientSocketFd, (char*)&command, sizeof(command), 0);
        switch (command) {
            case 'C':
                recv(clientSocketFd, fileName, sizeof(fileName), 0);
                createFile(clientSocketFd, fileName);
                break;
            case 'E':
                recv(clientSocketFd, fileName, sizeof(fileName), 0);
                editFile(clientSocketFd, fileName);
                break;
            case 'R':
                recv(clientSocketFd, fileName, sizeof(fileName), 0);
                removeFile(clientSocketFd, fileName);
                break;
            case 'L':
                listFile(clientSocketFd);
                break;
            case 'D':
                recv(clientSocketFd, fileName, sizeof(fileName), 0);
                downloadFile(clientSocketFd, fileName);
                break;
            case 'Q':       
                close(clientSocketFd);
                flag = 0;
                break;
        }
    }
    close(socketFd);
    return 0;
}
