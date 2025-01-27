/*
Byte based run length encoding

Copyright 2025 Ahmet Inan <xdsopl@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int putbyte(int byte) {
	return putchar_unlocked(byte) == EOF;
}

int getbyte() {
	return getchar_unlocked();
}

int encode() {
	int byte, prev = -1, count = -1;
	while ((byte = getbyte()) != EOF) {
		if (byte == 0 || byte == 255) {
			if (prev == byte) {
				if (count < 255) {
					++count;
				} else {
					if (putbyte(count))
						return 1;
					if (putbyte(byte))
						return 1;
					count = 0;
				}
			} else if (count < 0) {
				if (putbyte(byte))
					return 1;
				count = 0;
				prev = byte;
			} else {
				if (putbyte(count))
					return 1;
				if (putbyte(byte))
					return 1;
				count = 0;
				prev = byte;
			}
		} else {
			if (count >= 0) {
				if (putbyte(count))
					return 1;
				count = -1;
				prev = -1;
			}
			if (putbyte(byte))
				return 1;
		}
	}
	if (count >= 0 && putbyte(count))
		return 1;
	return 0;
}

int decode() {
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

