#ifndef COMMON_H
#define COMMON_H

#define PORT 9000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define MAX_PACKETS 100
#define WINDOW_SIZE 4

#define DATA 0
#define ACK  1
#define JOIN 2

typedef struct {
    int seq;
    int type;
    int cid;
    char msg[BUFFER_SIZE];
} Packet;

#endif