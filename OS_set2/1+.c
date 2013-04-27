// a curses chat 
// - a little more asynchronous then 1.c
// - press ESC to exit

#include <stdio.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <poll.h>

#define KEY_ESC 27
#define SOCK_NAME "/tmp/named_socket"
#define CLIENT -1
#define SERVER -2

int sockfd, confd, len;
struct sockaddr_un addr = { AF_UNIX, SOCK_NAME };

WINDOW *topb, *top, *bottomb, *bottom;

void init_curses()
{
	initscr();

	cbreak();
	keypad(stdscr, TRUE);
	
	topb = newwin(LINES-5, COLS, 0, 0);
	box(topb, 0, 0);
	wrefresh(topb);
	
	top = newwin(LINES-5-2, COLS-2, 1, 1);
	scrollok(top, TRUE);
	//wsetscrreg(top, 0, LINES-5-2);

	bottomb = newwin(5, COLS, LINES-5, 0);
	box(bottomb, 0, 0);
	wrefresh(bottomb);
	
	bottom = newwin(5-2, COLS-2, LINES-5+1, 1);
	nodelay(bottom, TRUE);
}

void term_curses()
{
	delwin(topb);
	delwin(top);
	delwin(bottomb);
	delwin(bottom);
	endwin();
}

int init_socket()
{
	struct stat s; // for stat
	
	sockfd = socket(PF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(sockfd == -1)
        return 1;

	if(stat(SOCK_NAME, &s) == 0) // assuming sock is created, we can become a client then
	{
		if(connect(sockfd, (struct sockaddr *) &addr, sizeof (struct sockaddr_un)) == -1)
			return 3;
		confd = sockfd;
		return CLIENT;
	}
	else //server
	{
		if(bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1)
			return 2;
		if(listen(sockfd, 1) == -1)
			return 3;
		return SERVER;
	}
}

void term_socket(int t)
{
	close(confd);

	if(t == SERVER)
		unlink(SOCK_NAME);
}

void snd(char *buf)
{
	len = strlen(buf);
    write(confd, buf, len);

	wprintw(top, ">%s", buf);
	wrefresh(top);
}

int rcv()
{
	int len;
	char buf[256];

	struct pollfd pfd;
    pfd.fd	= confd;
	pfd.events = POLLIN;

	if(poll(&pfd, 1, 0))
	{
		if(pfd.revents & POLLHUP) // connection closed
			return 1;
		len = recv(confd, buf, 256-1, MSG_DONTWAIT);
        buf[len] = 0;
        wprintw(top, "%s", buf);
    }
	wrefresh(top);
	return 0;
}

int communicate()
{
	char c, buf[256];
	int i;

	i = 0;
	while(1)
	{
		c = wgetch(bottom);
		if(rcv())
			return 1;
		else if(c != ERR)
		{
			if(c == KEY_ESC) 
				return 0;
			
			buf[i++] = c;
			
			if(c == '\n' || i >= 255)
			{
				buf[i] = 0;
				snd(buf);
				i = 0;
				wclear(bottom);
			}
		}
	}

}

int main()
{
	int t;

	init_curses();
	t = init_socket();

	if(t == CLIENT)
	{
		wprintw(top, "# Connected.\n");
		wrefresh(top); 
		if(communicate())
		{
			wprintw(top, "# The other party ended connection. Press any key.\n");
			wrefresh(top);
			while(wgetch(bottom) == ERR)
				usleep(100);
		}
	}
	else if(t == SERVER)
	{
		wprintw(top, "# Awaiting connections...\n");
		wrefresh(top);
		while(1)
		{
			if(wgetch(bottom) == KEY_ESC)
				break;
			len = sizeof(struct sockaddr_un);
			confd = accept(sockfd, (struct sockaddr *) &addr, &len);
			if(confd > -1)
			{
				wprintw(top, "# Got a connection!\n");
				wrefresh(top);
				if(communicate())
				{
					wprintw(top, "# The other party ended connection.\n");
					wrefresh(top);
				}
				//break;
			}
			usleep(100);
		}
	}
	else
		return t; // error

	term_socket(t);
	term_curses();
}
