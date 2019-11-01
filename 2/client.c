#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "config.h"

int main()
{
    int sock = 0;
    char message[MSG_LEN];
    char buf[MSG_LEN];
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    while (1) {
        printf("Enter index: ");
        memset(message, 0, sizeof(message));
        fgets(message, MSG_LEN, stdin);
        if (send(sock, message, sizeof(message), 0) < 0) {
            perror("send");
            exit(3);
        }

        if (recv(sock, buf, sizeof(message), 0) < 0) {
            perror("recv");
            exit(4);
        }

        printf("Message from server:\n");
        printf("%s\n", buf);
    }

    close(sock);

    return 0;
}