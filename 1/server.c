#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

static volatile char keepRunning = 1;

void CtrlCHandler(int code)
{
    keepRunning = 0;
}

int main(void)
{
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = CtrlCHandler;
    sigaction(SIGINT, &act, NULL);

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int slen = sizeof(client_addr);
    int sock = 0;
    char buf[MSG_LEN];

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock);
        exit(2);
    }

    printf("Server is listening\n\n");

    int maxLen = 0;
    while (keepRunning) {
        if (recvfrom(sock, buf, MSG_LEN, 0, (struct sockaddr*)&client_addr, &slen) == -1) {
            perror("recvfrom");
            break;
        }
        printf("Received message from %s:%d:\nData: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
        if (strlen(buf) > maxLen) {
            maxLen = strlen(buf);
        }
    }

    printf("Max length: %i\n", maxLen - 1);
    close(sock);

    return 0;
}
