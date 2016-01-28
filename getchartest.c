#include <stdio.h>
#include <unistd.h>

int main() {
    system("/bin/stty raw");
    char c[3] = {0};
    read(STDIN_FILENO, &c, 1);
    if (c[0] == 27) { // if this is an escape sequence
        read(STDIN_FILENO, &c[1], 2);
        printf("\nYou have entered (char 1): %d\n", c[1]);
        printf("\nYou have entered (char 2): %d\n", c[2]);
        switch (c[2]) {
            case 'A':
                printf("\nUP!\n");
                break;
            case 'B':
                printf("\nDOWN!\n");
                break;
            case 'C':
                printf("\nRIGHT!\n");
                break;
            case 'D':
                printf("\nLEFT!\n");
                break;
        }
    }
    else {
        printf("You entered: '%c' (%d)\n", c[0], c[0]);
    }
    system("/bin/stty cooked");
    return 1;
}
