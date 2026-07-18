/*** includes ***/

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s)
{
    perror(s);
    exit(1);
}

void disableRawMode()
{
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void enableRawMode()
{
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcsetattr");
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

    // Set min bytes to 0 and timeout for reading to 1 tenth of a second
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/

int main(void)
{
    enableRawMode();

    while (1) {
        char c = '\0';
        if(read(STDIN_FILENO, &c, sizeof(char)) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }

    return 0;
}
