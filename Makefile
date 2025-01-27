CFLAGS = -std=gnu99 -W -Wall -Ofast
# CFLAGS += -g -fsanitize=address

test: orleis
	./orleis e < orleis | ./orleis d | diff -q -s - orleis

clean:
	rm -f orleis

