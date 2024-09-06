#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "kiss.h"

/* 
 * Generate KISS encapsulated data
 */
int kiss_writeframe_old(int outfd, int infd) {
    /*
     * Processing is done in KISS_BUFSIZE-byte segments to avoid overrunning the TNC's buffer.
     * The output buffer's size will be doubled plus given two start/end bytes to assume
     * the larget possible packet full of escapes.
     */
    int packet_length = 0;
    int packet_position = 0;
    int packet_done = 0;
    unsigned char rx_buf[KISS_BUFSIZE];
    unsigned char tx_buf[KISS_BUFSIZE * 2 + 2];

    if((packet_length = read(infd, rx_buf, sizeof(rx_buf))) < 0) {
        fprintf(stderr, "NEU0600E Cannot read from input file descriptor %d\n", infd);
        close(outfd);
        exit(4);
    }

    if(packet_length == 0) return 1;
    
    /*
     * Now, the data has been acquired. A process will now be performed that will handle
     * escape sequences and prepend/append the frame marker byte.
     */
    tx_buf[packet_position++] = KISS_FEND;                   /* Frame marker */
    tx_buf[packet_position++] = KISS_COMMAND_CHAN0_SEND;     /* Command to send */

    /* Loop through and either handle escapes or copy the packet. */
    while(packet_done < packet_length) {
        if(rx_buf[packet_done] == KISS_FEND) {
            tx_buf[packet_position++] = KISS_FESC;
            tx_buf[packet_position++] = KISS_TFEND;
        }
        else if(rx_buf[packet_done] == KISS_FESC) {
            tx_buf[packet_position++] = KISS_FESC;
            tx_buf[packet_position++] = KISS_TFESC;
        }
        else {
            tx_buf[packet_position++] = rx_buf[packet_done];
        }
        packet_done++;
    }

    /* Put on the end byte. */
    tx_buf[packet_position] = KISS_FEND;    

    /* Send to TNC. */
    if(write(outfd, tx_buf, packet_position + 1) < 0) {
        fprintf(stderr, "NEU0600E Cannot write to output file descriptor %d\n", outfd);
        close(outfd);
        exit(5);
    }

    /* continue */
    return 0;
}

/* 
 * Generate KISS encapsulated data
 */
int kiss_encodeframe(unsigned char* tx_buf, unsigned char* rx_buf, int packet_length) {
    int packet_position = 0;
    int packet_done = 0;

    if(packet_length == 0 || packet_length > KISS_BUFSIZE) return -1;
    
    /*
     * Now, the data has been acquired. A process will now be performed that will handle
     * escape sequences and prepend/append the frame marker byte.
     */
    tx_buf[packet_position++] = KISS_FEND;                   /* Frame marker */
    tx_buf[packet_position++] = KISS_COMMAND_CHAN0_SEND;     /* Command to send */

    /* Loop through and either handle escapes or copy the packet. */
    while(packet_done < packet_length) {
        if(rx_buf[packet_done] == KISS_FEND) {
            tx_buf[packet_position++] = KISS_FESC;
            tx_buf[packet_position++] = KISS_TFEND;
        }
        else if(rx_buf[packet_done] == KISS_FESC) {
            tx_buf[packet_position++] = KISS_FESC;
            tx_buf[packet_position++] = KISS_TFESC;
        }
        else {
            tx_buf[packet_position++] = rx_buf[packet_done];
        }
        packet_done++;
    }

    /* Put on the end byte. */
    tx_buf[packet_position] = KISS_FEND;    

    /* continue */
    return packet_position;
}

/* 
 * Generate KISS encapsulated data
 */
int kiss_writeframe(int outfd, int infd) {
    /*
     * Processing is done in KISS_BUFSIZE-byte segments to avoid overrunning the TNC's buffer.
     * The output buffer's size will be doubled plus given two start/end bytes to assume
     * the larget possible packet full of escapes.
     */
    int packet_length;
    int processed_packet_length;
    unsigned char rx_buf[KISS_BUFSIZE];
    unsigned char tx_buf[KISS_BUFSIZE * 2 + 2];

    if((packet_length = read(infd, rx_buf, sizeof(rx_buf))) < 0) {
        fprintf(stderr, "NEU0600E Cannot read from input file descriptor %d\n", infd);
        close(outfd);
        exit(4);
    }

    processed_packet_length = kiss_encodeframe(tx_buf, rx_buf, packet_length);  
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
