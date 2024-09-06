#ifndef KISS_H
#define KISS_H 1

#define KISS_BUFSIZE 256

#define KISS_COMMAND_CHAN0_SEND 0x00
#define KISS_FEND 0xC0
#define KISS_FESC 0xDB
#define KISS_TFEND 0xDC
#define KISS_TFESC 0xDD

int kiss_writeframe_old(int outfd, int infd);
int kiss_encodeframe(unsigned char* tx_buf, unsigned char* rx_buf, int packet_length);
int kiss_writeframe(int outfd, int infd);

#endif
