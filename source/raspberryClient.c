/*
 ============================================================================
 Created on  : 2014. 9. 21.
 Name        : raspberryClient.c
 Author      : JeongHakOh
 Version     :
 Copyright   : JeongHakOh
 Description : C Language based Client, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Times.h"
//#include "serial.h"
#include "protocol.h"

#define BUF_SIZE 1024
#define MAX_SERVER 1
#define PORT 1234
#define IP1 "123.123.123.123"
#define IP2 "123.123.123.123"

int cntable[MAX_SERVER] = { 0, };

void error_handling(char *message);
void read_routine(/*int fd,*/ int sock, unsigned char *buf);
void ctrl_childproc(int sig);

int main(int argc, char *argv[]) {
	int sock[MAX_SERVER];
	int i = 0;

	pid_t pid;
	struct sigaction act;
	unsigned char buf[BUF_SIZE] = { 0, };
	struct sockaddr_in server_addr[MAX_SERVER];

	if (argc != ((2 * MAX_SERVER) + 1)) {
		printf("Usage : %s <IP1> <port> <IP2> <port>\n", argv[0]);
		exit(1);
	}

	//serial open
	//int serial_fd = serialOpen();

	// signal setting
	act.sa_handler = ctrl_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGCHLD, &act, 0) != 0)
		error_handling("LOG ( Sigaction() error. )\n");

	bzero((char *) &server_addr, sizeof(server_addr));
	for (i = 0; i < MAX_SERVER; i++) {

		// socket create
		if ((sock[i] = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			error_handling("LOG ( Can't open stream socket. )\n");

		// socket addr setting
		server_addr[i].sin_family = AF_INET;
		server_addr[i].sin_addr.s_addr = inet_addr(argv[((2 * i) + 1)]);
		server_addr[i].sin_port = htons(atoi(argv[((2 * i) + 2)]));
	}

	while (1) {

		for (i = 0; i < MAX_SERVER; i++) {
			if (cntable[i] == 0) {

				//test 5sec...
				//usleep(5000000);

				if (connect(sock[i], (struct sockaddr*) &server_addr[i],
						sizeof(server_addr[i])) == -1) {
					puts("LOG ( connect() error. )\n");

				} else
					puts("LOG ( New client connected... )");

				if ((pid = fork())) {
					cntable[i] = pid;
					printf("in child : %d\n", pid);
				}

				if (pid == 0)
					read_routine(/*serial_fd,*/ sock[i], buf);

			}
		}
	}

	for (i = 0; i < MAX_SERVER; i++)
		close(sock[i]);

	//serialClose(serial_fd);

	return 0;
}

void read_routine(/*int fd,*/ int sock, unsigned char *buf) {

	int tempSec = getSec();
	int OESec = tempSec % 2;
	printf("connetion time -> sec : %d, odd-even : %d\n", tempSec, OESec);

	while (1) {
		int str_len = read(sock, buf, BUF_SIZE);
		if (str_len == 0)
			return;

		dataCheck(str_len, buf);

		buf[str_len] = 0;
		printf("Message from server: %s\n", buf);
		//serialWrite(fd, buf);
	}
}

void ctrl_childproc(int sig) {
	pid_t pid;
	int status;
	int i = 0;
	pid = waitpid(-1, &status, WNOHANG);
	printf("LOG ( Removed proc id: %d ) \n", pid);
	for (i = 0; i < MAX_SERVER; i++) {
		if (cntable[i] == pid) {
			printf("LOG ( Removed cntable id: %d ) \n", cntable[i]);
			cntable[i] = 0;
		}
	}
}

void error_handling(char *message) {
	fputs(message, stderr);
	exit(1);
}
