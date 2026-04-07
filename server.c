#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include "common.h"

// encryption
void enc(char *msg) {
    for (int i = 0; i < strlen(msg); i++) {
        msg[i] ^= 0xAA;
    }
}

typedef struct {
    struct sockaddr_in addr;
} Client;

Client list[MAX_CLIENTS];
int total = 0;

int ack[MAX_CLIENTS][MAX_PACKETS] = {0};

int find(struct sockaddr_in *a) {
    for (int i = 0; i < total; i++) {
        if (list[i].addr.sin_addr.s_addr == a->sin_addr.s_addr &&
            list[i].addr.sin_port == a->sin_port) {
            return i;
        }
    }
    return -1;
}

int add(struct sockaddr_in *a) {
    if (total >= MAX_CLIENTS) return -1;

    list[total].addr = *a;
    printf("new client %d\n", total);

    return total++;
}

int main() {
    int sock;
    struct sockaddr_in serv, cli;
    socklen_t len = sizeof(cli);

    Packet p;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&serv, sizeof(serv));

    printf("server started...\n");
    printf("waiting clients...\n");

    while (total < 5) {
        recvfrom(sock, &p, sizeof(p), 0,
                 (struct sockaddr*)&cli, &len);

        int id = find(&cli);
        if (id == -1) add(&cli);
    }

    printf("start sending...\n");

    Packet arr[MAX_PACKETS];
    int n = 10;

    for (int i = 0; i < n; i++) {
        arr[i].seq = i;
        arr[i].type = DATA;

        sprintf(arr[i].msg, "msg %d", i);

        enc(arr[i].msg); 

        printf("encrypted %d: %s\n", i, arr[i].msg); 
    }

    struct timeval t;
    t.tv_sec = 2;
    t.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));

    int base = 0, next = 0;

    while (base < n) {

        while (next < base + WINDOW_SIZE && next < n) {

            for (int c = 0; c < total; c++) {

                if (c == 1 && next == 2) {
                    printf("drop %d for %d\n", next, c);
                    continue;
                }

                sendto(sock, &arr[next], sizeof(Packet), 0,
                       (struct sockaddr*)&list[c].addr,
                       sizeof(list[c].addr));
            }

            printf("sent %d\n", next);
            next++;
        }

        Packet ackp;
        int r = recvfrom(sock, &ackp, sizeof(ackp), 0,
                         (struct sockaddr*)&cli, &len);

        if (r < 0) {
            printf("timeout resend...\n");

            for (int c = 0; c < total; c++) {
                for (int i = base; i < next; i++) {
                    if (ack[c][i] == 0) {

                        sendto(sock, &arr[i], sizeof(Packet), 0,
                               (struct sockaddr*)&list[c].addr,
                               sizeof(list[c].addr));

                        printf("resend %d to %d\n", i, c);
                    }
                }
            }
        } else {
            if (ackp.type == ACK) {
                int cid = find(&cli);
                int s = ackp.seq;

                if (cid != -1) {
                    ack[cid][s] = 1;
                    printf("ack %d from %d\n", s, cid);
                }
            }
        }

        int ok = 1;

        for (int c = 0; c < total; c++) {
            if (ack[c][base] == 0) {
                ok = 0;
                break;
            }
        }

        if (ok) base++;
    }

    printf("done\n");

    close(sock);
    return 0;
}