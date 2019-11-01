#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"

void perror_and_exit(char* s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in server_addr;
    int sock, slen = sizeof(server_addr);
    char buf[MSG_LEN];

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);

    if (inet_aton(SRV_IP, &server_addr.sin_addr) == 0) {
        perror("inet_aton");
        exit(2);
    }

    while (1) {
        printf("Please, enter the message: ");

        fgets(buf, MSG_LEN, stdin);
        if (sendto(sock, buf, MSG_LEN, 0, &server_addr, slen) == -1) {
            perror("sendto");
            exit(3);
        }
        memset(buf, 0, sizeof(buf));
    }

    close(sock);
    return 0;
}