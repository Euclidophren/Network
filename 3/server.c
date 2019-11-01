#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config.h"

static volatile char keepRunning = 1;

void CtrlCHandler(int code)
{
    keepRunning = 0;
}

int writeToBinaryFile(const char* filename, const unsigned char* bytes)
{
    printf("Writing to binary file\n");
    const unsigned char *size = bytes;
    // printf("First bytes: \n");
    // for (int i = 0; i < 10; i++) {
    //     printf("%d\n", bytes[i]);
    // }
    while (*size != 0) {
        ++size;
    }
    long int max_size = strtol(bytes, NULL, 10);
    printf("Size: %ld\n", max_size);
    FILE* f = fopen(filename, "wb");
    if (!f) {
        return -1;
    }
    while (*size == 0) {
        ++size;
    }
    fwrite(size, 1, max_size, f);
    fclose(f);
    return 0;
}

int sendSizeMessage(int s, struct sockaddr_in clientAddr)
{
    printf("Sending message back!\n");
    if (sendto(s, MAX_FILE_BYTE_SIZE_STR, sizeof(MAX_FILE_BYTE_SIZE_STR), 0, (const struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    struct sigaction act;
    act.sa_handler = CtrlCHandler;
    sigaction(SIGINT, &act, NULL);

    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    unsigned char msg[MAX_FILE_BYTE_SIZE + strlen(MAX_FILE_BYTE_SIZE_STR) + 1];
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("sock");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SOCK_SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        close(sock);
        exit(2);
    }

    int clientAddrSize = sizeof(clientAddr);

    printf("Server is listenning up to %lu bytes\n", MAX_FILE_BYTE_SIZE + strlen(MAX_FILE_BYTE_SIZE_STR) + 1);

    while (1) {
        int recvSize = 0;
        if ((recvSize = recvfrom(sock, msg, MAX_FILE_BYTE_SIZE + strlen(MAX_FILE_BYTE_SIZE_STR) + 1, 0, (struct sockaddr*)&clientAddr, (socklen_t*)&clientAddrSize)) == -1) {
            perror("recvfrom");
            close(sock);
            exit(3);
        }

        msg[recvSize] = 0;
        printf("Received message: %s\n", msg);

        if (strcmp((const char*)msg, CLIENT_MESSAGE_GIVE_FILE_SIZE) == 0) {
            if (sendSizeMessage(sock, clientAddr)) {
                perror("sendto");
                close(sock);
                exit(4);
            }
            printf("Server message was sent\n");
        } else {
            if (writeToBinaryFile(OUTPUT_FILENAME, msg)) {
                perror("writeToBinaryFile");
                close(sock);
                exit(5);
            }

            printf("Wrote to file %s\n", OUTPUT_FILENAME);
        }
    }

    close(sock);
    return 0;
}
