/*
 * Implementation of Gap Buffer Text Storage, adapted for multiple cursors.
 *
 * AUTHOR: ETHAN CHENG <elc1798>
 *
 *
 * Theory:
 *
 *  In a traditional gap buffer, we can store a string in the fashion:
 *  ['h', 'e', 'l', 'l', 'o', '\n' , 'w', 0, 0, 0, 0, 0, 'o', 'r', 'l', 'd']
 *  This displays the:
 *  hello
 *  world
 *  With the cursor over the 'o' in "world". If the 'p' character is entered,
 *  the closest NULL byte to 'o' on its left will be turned into 'p', resulting
 *  in the buffer:
 *  ['h', 'e', 'l', 'l', 'o', '\n' , 'w', 0, 0, 0, 0, 'p', 'o', 'r', 'l', 'd']
 *
 *  However, this poses limitations, as you cannot differentiate between
 *  cursors. We can instead make the buffer a 2D array. Each element will be in
 *  the format: [CHARACTER, OWNER_ID], where the data type of this gap buffer
 *  will be:
 *
 *  unsigned char buffer[128][2]
 *
 *  Using unsigned char allows us to maintain the character set, as well as do
 *  some clever encoding:
 *
 *      We can limit the number of editors per room to 8, a reasonable number.
 *      Storing the editors (users) in an array, indices 0-7, denoted as 'i',
 *      we can mark down the location of a cursor by flipping the ith bit of the
 *      OWNER_ID (buffer[n][1] for the nth character) to true.
 *
 *      Ex:
 *          Users: [ASH, BOB, CRIS, DAVID, ETHAN, FRITZ, GERALD, 0] (0 denotes
 *          an empty slot)
 *
 *          An element of buffer, [0, 00001011] denotes a cursor location, since
 *          the index 0 is a NULL byte. The cursors of ASH, BOB, and DAVID all
 *          overlap on this position, since the 0th, 1st, and 3rd bits are
 *          flipped.
 *
 *  We will be implementing the buffer as an unbounded array: When we exceed
 *  more than 7N/8 characters (N, in this case, is 128), the buffer will double
 *  in size, the data copied over, the rest padded with default values (see
 *  below), and previous memory freed.
 */
