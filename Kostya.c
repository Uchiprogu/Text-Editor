/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)
/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
 if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);					// cьрасывает артибуты терминала при выходе 

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);							//Disable Ctrl-S and Ctrl-Q and Ctrl-M
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 			// be reading input byte-by-byte, instead of line-by-line.; Disable Ctrl-V

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}


/*** init ***/
int main() {
  enableRawMode();

  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
  	if(iscntrl(c))			//tests whether a character is a control character.
  	{
  		printf("%d\r\n", c);
  	} else{
  	    printf("%d ('%c')\r\n", c, c);	//%d tells it to format the byte as a decimal number (its ASCII code),
  	    								// and %c tells it to write out the byte directly, as a character.
  	}
  	if (c == CTRL_KEY('q')) break;
  }
  return 0;
}