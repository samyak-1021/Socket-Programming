#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "common.h"

// encryption + decryption
void enc(char *msg) {
    for (int i = 0; i < strlen(msg); i++) {
        msg[i] ^= 0xAA;
    }
}

int main() {
    int sock;
    struct sockaddr_in serv;

    Packet p, ack;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket error");
        exit(1);
    }

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    // join request
    memset(&p, 0, sizeof(p));
    p.type = JOIN;

    sendto(sock, &p, sizeof(p), 0,
           (struct sockaddr*)&serv, sizeof(serv));

    printf("client started...\n");

    int expect = 0;

    while (1) {
        memset(&p, 0, sizeof(p));

        int n = recvfrom(sock, &p, sizeof(p), 0, NULL, NULL);
        if (n < 0) continue;

        if (p.type == DATA) {

            enc(p.msg);

            if (p.seq == expect) {

                printf("decrypted %d: %s\n", p.seq, p.msg);

                ack.seq = p.seq;
                ack.type = ACK;

                sendto(sock, &ack, sizeof(ack), 0,
                       (struct sockaddr*)&serv,
                       sizeof(serv));

                expect++;

            } else {

                printf("skip %d (want %d)\n", p.seq, expect);

                ack.seq = expect - 1;
                ack.type = ACK;

                sendto(sock, &ack, sizeof(ack), 0,
                       (struct sockaddr*)&serv,
                       sizeof(serv));
            }
        }
    }

    close(sock);
    return 0;
}