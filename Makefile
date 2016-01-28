build:
	gcc -o server.out central.c protocols.c
	gcc -o client.out editor.c editor_backend.c gap_buffer.c text_buffer.c -lncurses

clean:
	rm *.out
