#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)       \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

static volatile char keepRunning = 1;

void CtrlCHandler(int code)
{
    keepRunning = 0;
}

int main()
{
    struct sigaction act;
    act.sa_handler = CtrlCHandler;
    sigaction(SIGINT, &act, NULL);

    int sock = 0;
    int listener = 0;
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    char answer[MSG_LEN];
    char recvBuf[MSG_LEN];
    int bytes_read = 0;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        exit(2);
    }

    listen(listener, 1);

    printf("Server is listening\n\n");

    FILE* f = fopen("ip4", "r");

    while (keepRunning) {
        socklen_t cli_addr_size = sizeof(client_addr);
        sock = accept(listener, (struct sockaddr*)&client_addr, &cli_addr_size);
        if (sock < 0) {
            perror("accept");
            close(listener);
            exit(3);
        }

        printf("Opened connection %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        while (1) {
            memset(recvBuf, 0, sizeof(answer));
            bytes_read = recv(sock, recvBuf, MSG_LEN, 0);
            if (bytes_read <= 0) {
                break;
            }
            memset(answer, 0, sizeof(answer));
            int index = atoi(recvBuf);
            if (index < 1) {
                fseek(f, 0, SEEK_SET);
                fread(answer, sizeof(char), MSG_LEN, f);
            } else {
                --index;
                fseek(f, index * 16, SEEK_SET);
                char ipv4[16] = { 0x0 };
                char* fgetsRes = fgets(ipv4, 16, f);
                if (fgetsRes == NULL) {
                    sprintf(answer, "%s", "Not found");
                } else {
                    ipv4[3] = '\0';
                    ipv4[7] = '\0';
                    ipv4[11] = '\0';

                    int first = atoi(ipv4);
                    int second = atoi(ipv4 + 4);
                    int third = atoi(ipv4 + 8);
                    int fourth = atoi(ipv4 + 12);
                    sprintf(answer, "%02x.%02x.%02x.%02x\n" BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN, first, second, third, fourth, BYTE_TO_BINARY(first), BYTE_TO_BINARY(second), BYTE_TO_BINARY(third), BYTE_TO_BINARY(fourth));
                }
            }
            send(sock, answer, MSG_LEN, 0);
        }
        close(sock);
    }

    close(listener);

    return 0;
}
