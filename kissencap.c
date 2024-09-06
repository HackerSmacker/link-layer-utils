#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "kiss.h"

int main(int argc, char* argv[]) {
    struct sockaddr_in kisstnc;
    int kissfd = 0;

    if(argc < 2) {
        fprintf(stderr, "NEU0500E Bad arguments specified\n");
        fprintf(stderr, "usage: %s [ip] [port]\n", argv[0]);
        fprintf(stderr, "       %s [tty]\n", argv[0]);
        fprintf(stderr, "where [ip] is the IP address of a machine running a KISS TNC, and [port] is the TCP port it is listening on\n");
        fprintf(stderr, "or, where [tty] is a character device file your KISS TNC is on\n");
        exit(1);
    } 

    if(argc == 2) {
        kissfd = open(argv[1], O_RDWR);
        if(kissfd < 0) {
            fprintf(stderr, "NEU0530E Cannot open KISS TNC output device/file %s\n", argv[1]);
            exit(2);
        }
        fprintf(stderr, "NEU0600I KISS started, destination device is %s\n", argv[1]);
    }
    else if(argc == 3) {
        kisstnc.sin_family = AF_INET;
        kisstnc.sin_port = htons(atoi(argv[2]));
        kisstnc.sin_addr.s_addr = inet_addr(argv[1]);
        kissfd = socket(AF_INET, SOCK_STREAM, 0);
        if(kissfd == 0) {
            fprintf(stderr, "NEU0510E Failed to create client socket\n");
            exit(2);
        }
        if(connect(kissfd, (struct sockaddr*) &kisstnc, sizeof(kisstnc)) < 0) {
            fprintf(stderr, "NEU0520E Cannot connect to TNC at %s:%s\n", argv[1], argv[2]);
            exit(3);
        }
        fprintf(stderr, "NEU0600I KISS started, destination TCP socket is %s:%s\n", argv[1], argv[2]);
    }
    else {
        fprintf(stderr, "NEU0500E Too many arguments specified\n");
    }


    do {
        putchar('.');
    }
    while(kiss_writeframe(kissfd, STDIN_FILENO) != 1);

    close(kissfd);

    return 0;
}
