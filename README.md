# Go-Back-N Protocol with Multi-Client Support (UDP + Basic Security)

## Overview

This project implements the Go-Back-N (GBN) protocol over UDP to achieve reliable data transfer.  
Since UDP is unreliable, we added our own logic for ordering, acknowledgments, and retransmission.

The server sends packets to multiple clients, and each client ensures in-order delivery.  
We also added a simple encryption layer to simulate secure communication.

---

## Features

- Go-Back-N protocol implementation
- Sliding window (window size = 4)
- Multi-client support
- Packet loss simulation
- Timeout and retransmission
- Basic encryption using XOR

---

## Project Structure

.
├── server.c
├── client.c
├── common.h
└── README.md

---

## How It Works

1. Server waits for clients to join  
2. Clients send a JOIN request  
3. Server starts sending packets using a sliding window  
4. Clients:
   - accept only in-order packets  
   - send ACKs  
   - discard out-of-order packets  
5. Server:
   - tracks ACKs for each client  
   - retransmits missing packets  
6. Data is encrypted before sending and decrypted at client  

---

## Security (Basic)

We added a simple XOR-based encryption:

- Server encrypts data before sending  
- Client decrypts data after receiving  

This is a lightweight approach to demonstrate secure communication similar to DTLS concepts.

---

## Compilation (Mac/Linux)

Open terminal in project folder:

gcc server.c -o server
gcc client.c -o client

---

## Running the Project

Step 1: Start Server

./server

Step 2: Start Clients (in separate terminals)

./client

Run at least 2 clients.

---

## Sample Output

Server:

server started...
waiting clients...
enc 0: ...
sent 0

Client:

client started...
decrypted 0: msg 0

---

## Packet Loss Simulation

The server intentionally drops one packet for a client to demonstrate retransmission.

---

## Limitations

- Uses simple XOR encryption (not secure for real-world use)
- No authentication or integrity checks
- Designed for learning purposes

---

## Future Improvements

- Implement full DTLS using OpenSSL
- Add checksum or hashing for integrity
- Improve performance for large-scale systems

---

## Team Contribution

- Member 1: Server-side GBN logic and sliding window  
- Member 2: Client-side receiver logic and ACK handling  
- Member 3: Multi-client handling, retransmission logic, and encryption  

---

## Conclusion

This project demonstrates how reliable communication can be built on top of UDP using Go-Back-N protocol, along with basic security concepts.