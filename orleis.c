/*
Byte based adaptive run length encoding

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

int put_leb128(int value)
{
	while (value >= 128) {
		if (putbyte((value & 127) | 128))
			return 1;
		value >>= 7;
	}
	return putbyte(value);
}

int get_leb128()
{
	int byte, shift = 0, value = 0;
	while ((byte = getbyte()) >= 128) {
		value |= (byte & 127) << shift;
		shift += 7;
	}
	if (byte < 0)
		return byte;
	return value | (byte << shift);
}

static char score[256];

void update(int prev, int byte) {
	if (byte == prev) {
		if (score[byte] < 127)
			++score[byte];
	} else {
		if (score[byte] > -128)
			--score[byte];
	}
}

int good(int byte) {
	return score[byte] > 0;
}

int encode() {
	if (fill_ibuffer())
		return 0;
	int byte, prev = getbyte(), count = 0;
	if (prev == EOF)
		return 0;
	if (putbyte(prev))
		return 1;
	while ((byte = getbyte()) != EOF) {
		if (prev == byte && count < 268435455) {
			if (good(byte)) {
				++count;
				continue;
			}
		} else {
			if (good(prev) && put_leb128(count))
				return 1;
			count = 0;
		}
		if (putbyte(byte))
			return 1;
		update(prev, byte);
		prev = byte;
	}
	if (good(prev) && put_leb128(count))
		return 1;
	if (flush_obuffer())
		return 1;
	return 0;
}

int decode() {
	if (fill_ibuffer())
		return 0;
	int byte, prev = getbyte();
	if (prev == EOF)
		return 0;
	if (putbyte(prev))
		return 1;
	while ((byte = getbyte()) != EOF) {
		update(prev, byte);
		if (putbyte(byte))
			return 1;
		if (good(byte)) {
			int count = get_leb128();
			if (count == EOF)
				return 1;
			while (count--)
				if (putbyte(byte))
					return 1;
		}
		prev = byte;
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

