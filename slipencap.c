#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "slip.h"

int main(int argc, char* argv[]) {
    struct sockaddr_in sliptnc;
    int slipfd = 0;

    if(argc < 2) {
        fprintf(stderr, "NEU0500E Bad arguments specified\n");
        fprintf(stderr, "usage: %s [ip] [port]\n", argv[0]);
        fprintf(stderr, "       %s [tty]\n", argv[0]);
        fprintf(stderr, "where [ip] is the IP address of a machine accepting SLIP over TCP, and [port] is the TCP port it is listening on\n");
        fprintf(stderr, "or, where [tty] is a character device file your remote system is connected to\n");
        exit(1);
    } 

    if(argc == 2) {
        slipfd = open(argv[1], O_RDWR);
        if(slipfd < 0) {
            fprintf(stderr, "NEU0530E Cannot open SLIP output device/file %s\n", argv[1]);
            exit(2);
        }
        fprintf(stderr, "NEU0600I SLIP started, destination device is %s\n", argv[1]);
    }
    else if(argc == 3) {
        sliptnc.sin_family = AF_INET;
        sliptnc.sin_port = htons(atoi(argv[2]));
        sliptnc.sin_addr.s_addr = inet_addr(argv[1]);
        slipfd = socket(AF_INET, SOCK_STREAM, 0);
        if(slipfd == 0) {
            fprintf(stderr, "NEU0510E Failed to create client socket\n");
            exit(2);
        }
        if(connect(slipfd, (struct sockaddr*) &sliptnc, sizeof(sliptnc)) < 0) {
            fprintf(stderr, "NEU0520E Cannot connect to SLIP server socket at %s:%s\n", argv[1], argv[2]);
            exit(3);
        }
        fprintf(stderr, "NEU0600I SLIP started, destination TCP socket is %s:%s\n", argv[1], argv[2]);
    }
    else {
        fprintf(stderr, "NEU0500E Too many arguments specified\n");
    }


    do {
        putchar('.');
    }
    while(slip_writeframe(slipfd, STDIN_FILENO) != 1);

    close(slipfd);

    return 0;
}
