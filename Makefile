build:
	gcc -o server.out central.c distribute.c protocols.c
	gcc -o client.out gap_buffer.c
