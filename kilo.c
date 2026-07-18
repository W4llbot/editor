#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    /**
     * IXON: turns off software flow control (Ctrl-S and Ctrl-Q)
     * ICRNL: turns off automatically translating CR to newline (fixes Ctrl-M)
     * Optional (legacy flags): BRKINT, INPCK, ISTRIP, CS8
    */
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);
    raw.c_cflag |= (CS8);
    // OPOST: turns off all output processing (EG: translating \n to \r\n)
    raw.c_oflag &= ~(OPOST);
    /**
     * ECHO: turns off echoing characters typed
     * ICANON: reads every byte instead of only on newline
     * ISIG: turns of Ctrl-Z and Ctrl-C
     * IEXTEN: turn off Ctrl-V for verbatim
    */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(void)
{
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, sizeof(char)) == 1 && c != 'q') {
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
    }

    return 0;
}
