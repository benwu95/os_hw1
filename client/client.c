#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_MAX 10000
#define MSG_MAX 20
#define FILE_NAME_MAX 256

void sendFileName(int socketFd, char fileName[]) {
    printf("Enter the name of the file: ");
    scanf("%s", fileName);
    send(socketFd, fileName, sizeof(char) * strlen(fileName), 0);
}

void receiveList(int socketFd) {
    char list[FILE_NAME_MAX] = {0};
    int bytes;
    while (recv(socketFd, list, sizeof(list), 0)) {
        if (strcmp(list, "\\") == 0)
            break;
        puts(list);
        memset(list, 0, sizeof(list));
    }
}

int receiveFile(int socketFd, char* fileName) {
    FILE* fp;
    char* buf = (char*) malloc (sizeof(char) * BUF_MAX);
    char* block = (char*) malloc (sizeof(char) * 2);
    memset(buf, 0, sizeof(buf));
    int bytes;
    /* Check file. */
    recv(socketFd, buf, sizeof(buf), 0);
    if (strncmp(buf, "NULL", 4) == 0)
        return 2;
    if (fopen(fileName, "r") != NULL)
        strcat(fileName, "(new)");
    /* Receive the size of the file. */
    int total = atoi(buf);
    memset(buf, 0, sizeof(buf));
    /* Receive the file. */
    fp = fopen(fileName, "wb");
    int sum = 0;
    int flag = 1;
    while (flag) {
        if (total == 0)
            break;
        recv(socketFd, buf, sizeof(buf), 0);
        bytes = atoi(buf);
        /* Block. */
        send(socketFd, block, sizeof(block), 0);
        /* Receive buffer. */
        recv(socketFd, buf, bytes, 0);
        fwrite(buf, sizeof(char), bytes, fp);
        if ((total - sum) <= bytes)
            flag = 0;
        sum+=bytes;
        printf("Downloading... %d/%d (bytes)\n", sum, total);
        memset(buf, 0, sizeof(buf));
    }
    puts("Success!");
    fclose(fp); 
    free(buf);
    free(block);
    return 0;
}

int main(int argc, char* argv[]) {
    char* socketAddr = argv[1];
    int port = atoi(argv[2]);
    int socketFd;
    struct sockaddr_in address;

    /* Create the socket. */
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    /* Store the server's address in the socket address. */
    address.sin_addr.s_addr = inet_addr(socketAddr);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    /* Connect to server. */
    if (connect(socketFd, (struct sockaddr*)&address, sizeof(struct sockaddr))
            == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    puts("///////////////////////////////////////////");
    puts("//       Welcome to Online Editor        //");
    puts("//                                       //");
    puts("// Choose options below:                 //");
    puts("//  (C)reate file                        //");
    puts("//  (E)dit file                          //");
    puts("//  (R)emove file                        //");
    puts("//  (L)ist file                          //");
    puts("//  (D)ownload file                      //");
    puts("//  (Q)uit editor                        //");
    puts("//                                       //");
    puts("///////////////////////////////////////////");
    
    int flag = 1;
    char command;
    char msg[MSG_MAX];
    //char* text = (char*) malloc (sizeof(char)* BUF_MAX);
    //memset(text, 0, sizeof(text));
    char fileName[FILE_NAME_MAX + 1];
    char* block = (char*) malloc (sizeof(char) * 2);
    while (flag) {
        memset(fileName, 0, sizeof(fileName));
        memset(msg, 0, sizeof(msg));
        printf("Please choose one option(C, E, R, L, D, Q): ");
        scanf("%c", &command);
        switch (command) {
            case 'C':
                send(socketFd, (char*)&command, sizeof(command), 0);
                sendFileName(socketFd, fileName);
                recv(socketFd, msg, sizeof(msg), 0);
                puts(msg);
                break;
            case 'E':
                send(socketFd, (char*)&command, sizeof(command), 0);
                sendFileName(socketFd, fileName);
                recv(socketFd, msg, sizeof(msg), 0);
                puts(msg);
                break;
            case 'R':
                send(socketFd, (char*)&command, sizeof(command), 0);
                sendFileName(socketFd, fileName);
                recv(socketFd, msg, sizeof(msg), 0);
                puts(msg);
                break;
            case 'L':
                send(socketFd, (char*)&command, sizeof(command), 0);
                receiveList(socketFd);
                break;
            case 'D':
                send(socketFd, (char*)&command, sizeof(command), 0);
                sendFileName(socketFd, fileName);
                /* Check status of the file. */
                int status = receiveFile(socketFd, fileName);
                if (status == 1)
                    puts("File already exists!");
                else if (status == 2)
                    puts("Can't find on server!");
                break;
            case 'Q':
                send(socketFd, (char*)&command, sizeof(command), 0);
                flag = 0;
                break;
            default:
                puts("Wrong option!");
        }
        getchar();
    }
    free(block);
    close(socketFd);
    return 0;
}
