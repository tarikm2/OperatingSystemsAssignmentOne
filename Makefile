CC=gcc
CFLAGS=-pthread -o

binaries=fileReporter

all: $(binaries)

fileReporter: fileReporter.c
	$(CC) $(CFLAGS) $@ $^ 

clean:
	$(RM) $(binaries) 
