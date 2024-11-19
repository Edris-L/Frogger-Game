#include <unistd.h>  // Provides access to the POSIX operating system API.
#include <termios.h> // Used for controlling terminal I/O characteristics.
#include <fcntl.h>   // Provides an interface for file descriptor manipulation.
#include <stdio.h>   // Standard input-output header.
#include <stdlib.h>  // Standard library for memory allocation, process control, etc>

// Game constants
#define WIDTH 64
#define HEIGHT 10

// Game global variables
int frogX, frogY;

char lanes[HEIGHT][WIDTH + 1] = {
    "xxx..xxx..xxx..xxxxxxxxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxx..xxxxx", // l a n e 0
    "...xxxx..xxxxxx.......xxxx.....xx...xxxx.....xxxxxx...xxxxx.....", // l a n e 1
    "....xxxx.....xxxx.....xxxx.......xxxxxxx.....xx....xxxxxx.......", // l a n e 2
    "..xxx.....xxx.....xxx.....xxx...xxx....xx....xxxx....xx......xx." , // l a n e 3
    "................................................................" , // l a n e 4
    "....xxxx.......xxxx.........xxxx.......xxxx......xxxx....xxxx..." , // l a n e 5
    ".....xx...xx...xx......xx....xx.......xx..xx.xx......xx.......xx", // l a n e 6
    "..xxx.....xx......xxxx..xx......xxxx......xxxx.......xxx...xxx.." , // l a n e 7
    "..xx.....xx.......xx.....xx.....xx..xx.xx........xx....xx......." , // l a n e 8
    "................................................................"  // l a n e 9
};


int speeds[HEIGHT] = {0, -2, +1, -1, 0, +2, -1, -1, +1, 0};

// declaring functions (if key is pressed, handle draw, handle updates)

int kbhit(void);
void draw();
void update();

int main() {
    // setting first position of frog to middle bottom of screen
    frogX = WIDTH / 2;
    frogY = HEIGHT - 1;

    while (1) {
        if (kbhit()) {
            // setting conditions regarding what key is pressed
            switch (getchar()) {
                case 'w': frogY--; break;
                case 'a': frogX--; break;
                case 's': frogY++; break;
                case 'd': frogX++; break;
                case 'q': exit(0);
            }
        }
        update(); // updates game state such as moving lanes, checking for crash with x
        draw();
        usleep(900000); // slowing down the game a bit

        // if frogger hits top lane and wins

        if (frogY == 0)
        {
            printf("\nYOU WIN!!!CONGRATS!\n");
            exit(0);
        }
    }
    return 0;
}

int kbhit(void) {
    struct termios oldt, newt;

    // variables to store character input and old file statues flags
    int ch;
    int oldf;

    //get current terminal settings and storing them to oldt (searched this up)
    tcgetattr(STDIN_FILENO, &oldt);
    // copy the old setting to newt so we can modify later
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // trying to read character from input
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    // if read character then put it back and return 1 (indicating key pressed)
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void draw() {
    system("clear"); //clears terminal screen

    // loop through each row and columin in row, and check if current position
    // is where frog is, if it is then print 'F', or else print the character
    // from the lanes array
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == frogX && y == frogY) {
                putchar('F');
            } else {
                putchar(lanes[y][x]);
            }
        }
        putchar('\n');
    }
}

void update() {
    // Check if the frogger hits an 'x'
    if (lanes[frogY][frogX] == 'x') {
        // Game over scenario
        printf("\nGame Over! You hit an 'x'.\n");
        exit(0);  // quit the game
    }

    // Update the lanes array based on speeds
    for (int y = 0; y < HEIGHT; y++) {
        if (speeds[y] != 0) { // only updating row is speed isnt zero
            char temp[WIDTH + 1]; // temporary array to hold updated row
            // if speed positive then move right
            if (speeds[y] > 0) {
                for (int x = 0; x < WIDTH; x++) {
                    // calculating new position and moving element there
                    temp[(x + speeds[y]) % WIDTH] = lanes[y][x];
                }
            } else { 
                // if speed negative then move left
                for (int x = 0; x < WIDTH; x++) {
                    // calculating new position and move element there
                    temp[x] = lanes[y][(x - speeds[y] + WIDTH) % WIDTH];
                }
            }
            // copying updated row back to original
            for (int x = 0; x < WIDTH; x++) {
                lanes[y][x] = temp[x];
            }
        }
    }
}