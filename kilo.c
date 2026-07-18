#include <unistd.h>
#include <termios.h>

void enableRawMode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(void)
{
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, sizeof(char)) == 1 && c != 'q');
    return 0;
}
