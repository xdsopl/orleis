/*
Byte based run length encoding

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define OBUFFER 65536
static size_t obytes;
static unsigned char obuffer[OBUFFER];

int putbyte(int byte) {
	if (obytes >= OBUFFER) {
		if (obytes != fwrite(obuffer, 1, OBUFFER, stdout))
			return 1;
		obytes = 0;
	}
	obuffer[obytes++] = byte;
	return 0;
}

int flush_obuffer() {
	return obytes > 0 && obytes != fwrite(obuffer, 1, obytes, stdout);
}

#define IBUFFER 65536
static size_t ibytes, ileft;
static unsigned char ibuffer[IBUFFER];

int fill_ibuffer() {
	return !(ileft = fread(ibuffer, 1, IBUFFER, stdin));
}

int getbyte() {
	if (ibytes >= ileft) {
		if (ileft < IBUFFER)
			return EOF;
		ileft = fread(ibuffer, 1, IBUFFER, stdin);
		if (ileft == 0)
			return EOF;
		ibytes = 0;
	}
	return ibuffer[ibytes++];
}

int encode() {
	if (fill_ibuffer())
		return 0;
	int byte, prev = getbyte(), count = 0;
	if (prev == EOF)
		return 1;
	while ((byte = getbyte()) != EOF) {
		if (prev == byte && count < 255) {
			++count;
		} else {
			if (putbyte(prev))
				return 1;
			if (prev == 0 || prev == 255) {
				if (putbyte(count))
					return 1;
			} else {
				while (count--)
					if (putbyte(prev))
						return 1;
			}
			count = 0;
		}
		prev = byte;
	}
	if (putbyte(prev))
		return 1;
	if ((prev == 0 || prev == 255) && putbyte(count))
		return 1;
	if (flush_obuffer())
		return 1;
	return 0;
}

int decode() {
	if (fill_ibuffer())
		return 0;
	int byte;
	while ((byte = getbyte()) != EOF) {
		if (putbyte(byte))
			return 1;
		if (byte == 0 || byte == 255) {
			int count = getbyte();
			if (count == EOF)
				return 1;
			while (count--)
				if (putbyte(byte))
					return 1;
		}
	}
	if (flush_obuffer())
		return 1;
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	if (*argv[1] == 'e')
		return encode();
	if (*argv[1] == 'd')
		return decode();
	return 1;
}

