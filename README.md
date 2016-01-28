# NetScribe
Cross-Network Collaboatory text-editor with built-in chat client.

## Problem
We seek to solve the problem with the lack of effective co-coding engine. The
project will allow many people to collaborate on the same piece of code at once
with ease.

## UI
The user interface looks somewhat like nano, with a title bar on top, a
chat/status bar below the title bar to display incoming chat messages and system
informations. Below that is the buffer for editing, below that is the
instruction for key bindings, sporting ^X to exit, ^L to clear, ^O to save and
^H to enter chat mode. The UI is backed by ncurses.

## Technical Design

### Server Structure
The server is a multi-room distribution server. It has a list of client struct,
with fields of name, which is the name of the client, room_id, which the id of
the client in the room, room, which is the room number that the client is in,
and socket_id, which is the WRITE_ONLY socket from the server to the client.
These fields are set when the client joins the server and either creates/joins a
room. If the client creates a room, he/she is the owner of the room, and if he/she
quits the room is 'destroyed.' Now after the basic parameters are set, every
move of the client (moving cursor, deleting character, inserting character, chat
message) will be sent to the server via a new connection in a message struct.
The server then distributes the message struct via write-only pipes it has on
each client, and all the clients process instructions on the client side.

### Client Structure
The client side program is, in its essense, a multi-cursor gap buffer editor.
Usually in a gap buffer, the cursor is denoted by a gap. Here, we need multiple
gaps, so in each gap we have a 2D array, one of which indicates the index of the
gap in the file, the other is a bit-switched signal for which user is at this
location, more information can be found in the theory block on top of
gap_buffer.c

## Dev Team
- Ethan Cheng (pd. 4) -- backend gap buffer/text-editor, front-end text editor
- Sophia Zheng (pd. 4) -- backend gap buffer
- Ruojia Sun (pd. 5) -- distribution server and server protocols 
- Yicheng Wang (pd. 5) -- central server structure, client side networking,
  front-end text editor

## Files
To build both files simply run make, to get documentation, run:

```
./client.out -h
```
```
./server.out -h
```

## Current State
Currently the network part of the project works (join server, create room, join
room, and exit room are all working, which implies that the distribution
algorithms are working as join and exit requires distribution). However, the
front end of the text editor, which is written with the help of ncurses, is not
working. To test one can run
```
./server.out -d
```
```
./client.out -n <name> -t
```
