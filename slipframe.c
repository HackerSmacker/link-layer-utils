#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "slip.h"

/* 
 * Generate SLIP encapsulated data
 */
int slip_encodeframe(unsigned char* tx_buf, unsigned char* rx_buf, int packet_length) {
    int packet_position = 0;
    int packet_done = 0;

    /* 
     * Return an invalid packet length if the parameters are bad.
     */
    if(packet_length == 0 || packet_length > SLIP_BUFSIZE) return -1;
    
    /*
     * Now, the data has been acquired. A process will now be performed that will handle
     * escape sequences and prepend/append the frame marker byte.
     */
    tx_buf[packet_position++] = SLIP_FEND;                   /* Frame marker */

    /* Loop through and either handle escapes or copy the packet. */
    while(packet_done < packet_length) {
        if(rx_buf[packet_done] == SLIP_FEND) {
            tx_buf[packet_position++] = SLIP_FESC;
            tx_buf[packet_position++] = SLIP_TFEND;
        }
        else if(rx_buf[packet_done] == SLIP_FESC) {
            tx_buf[packet_position++] = SLIP_FESC;
            tx_buf[packet_position++] = SLIP_TFESC;
        }
        else {
            tx_buf[packet_position++] = rx_buf[packet_done];
        }
        packet_done++;
    }

    /* Put on the end byte. */
    tx_buf[packet_position] = SLIP_FEND;    

    /* continue */
    return packet_position;
}

/* 
 * Generate SLIP encapsulated data
 */
int slip_writeframe(int outfd, int infd) {
    /*
     * Processing is done in SLIP_BUFSIZE-byte segments to avoid overrunning the SLIP MTU.
     * The output buffer's size will be doubled plus given one start/end byte to assume
     * the larget possible packet full of escapes.
     */
    int packet_length;
    int processed_packet_length;
    unsigned char rx_buf[SLIP_BUFSIZE];
    unsigned char tx_buf[SLIP_BUFSIZE * 2 + 2];

    if((packet_length = read(infd, rx_buf, sizeof(rx_buf))) < 0) {
        fprintf(stderr, "NEU0600E Cannot read from input file descriptor %d\n", infd);
        close(outfd);
        exit(4);
    }

    processed_packet_length = slip_encodeframe(tx_buf, rx_buf, packet_length);  
    if(processed_packet_length < 0) return 1;

    /* Send to TNC. */
    if(write(outfd, tx_buf, processed_packet_length + 1) < 0) {
        fprintf(stderr, "NEU0600E Cannot write to output file descriptor %d\n", outfd);
        close(outfd);
        exit(5);
    }

    /* continue */
    return 0;
}
