#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config.h"

int readLimitedBinaryFile(const char* filename, int limit, unsigned char* buf)
{
    FILE* f = fopen(filename, "rb");
    if (!f) {
        return 0;
    }

    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    if (size > limit) {
        free(buf);
        return -1;
    }

    int offset = strlen(MAX_FILE_BYTE_SIZE_STR) + 1;

    char byte;
    int i = 0;
    while (fread(&byte, 1, 1, f)) {
        buf[offset + i] = byte;
        i++;
    }

    fclose(f);

    sprintf(buf, "%d", i);

    return i;
}

int main(void)
{
    int sock = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SOCK_SERVER_PORT);
    if (inet_aton(SOCK_ADDR, (struct in_addr*)&serverAddr.sin_addr) == 0) {
        perror("inet_aton");
        close(sock);
        exit(2);
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(SOCK_CLIENT_PORT);
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (const struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) {
        perror("bind");
        close(sock);
        exit(3);
    }

    if (sendto(sock, CLIENT_MESSAGE_GIVE_FILE_SIZE, strlen(CLIENT_MESSAGE_GIVE_FILE_SIZE), 0, (const struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("sendto");
        close(sock);
        exit(4);
    }

    printf("Message send\n");

    char msg[strlen(MAX_FILE_BYTE_SIZE_STR)];
    int recSize = recv(sock, msg, strlen(MAX_FILE_BYTE_SIZE_STR), 0);
    if (recSize < 0) {
        perror("recv");
        close(sock);
        exit(5);
    }

    msg[recSize] = 0;
    printf("Received message: %s\n", msg);

    long int maxSize = strtol(msg, NULL, 10);

    while (1) {
        char filename[100] = { 0x0 };
        scanf("%s", filename);

        unsigned char* ch = calloc(maxSize + strlen(MAX_FILE_BYTE_SIZE_STR), 1);
        int fileLen = readLimitedBinaryFile(filename, maxSize, ch);
        if (!fileLen) {
            perror("fread");
            close(sock);
            exit(6);
        }

        if (fileLen == -1) {
            printf("File is too big\n");
            continue;
        }
        printf("File loaded with size: %d\n", fileLen);

        printf("Sending message\n");
        if (sendto(sock, ch, fileLen + strlen(MAX_FILE_BYTE_SIZE_STR) + 1, 0, (const struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            close(sock);
            close(clientAddr);
            free(ch);
            perror("sendto");
            exit(9);
        }

        printf("Message was sent\n");
        free(ch);
        close(clientAddr);
    }

    return 0;
}