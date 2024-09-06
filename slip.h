#ifndef SLIP_H
#define SLIP_H 1

#define SLIP_BUFSIZE 1524

#define SLIP_FEND 0xC0
#define SLIP_FESC 0xDB
#define SLIP_TFEND 0xDC
#define SLIP_TFESC 0xDD

int slip_encodeframe(unsigned char* tx_buf, unsigned char* rx_buf, int packet_length);
int slip_writeframe(int outfd, int infd);

#endif
