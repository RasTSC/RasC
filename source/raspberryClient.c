/*
 ============================================================================
 Created on	:	2014. 9. 21.
 Name			:	raspberryClient.c
 Author		:	JeongHakOh
 Version		:
 Copyright		:	JeongHakOh
 Description	:	C Language based Client, Ansi-style
 URL			:	https://github.com/RasTSC/RasC
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Times.h"
//#include "serial.h"
#include "protocol.h"
#include "filectrl.h"

#define BUF_SIZE 1024
#define MAX_SERVER 1
#define PORT 1234
#define IP1 "123.123.123.123"
#define IP2 "123.123.123.123"
#define ENGINEROOM "log/engine/e_"
#define BRIDGEROOM "log/bridge/b_"

int cntable[MAX_SERVER] = { 0, };

void read_routine(/*int fd,*/int sock, unsigned char *buf, int servType);
void ctrl_childproc(int sig);
void error_handling(char *message);

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

	// make log file dir....
	mkdir("log", 0777);
	mkdir("log/engine", 0777);
	mkdir("log/bridge", 0777);

	// signal setting
	act.sa_handler = ctrl_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGCHLD, &act, 0) != 0)
		error_handling("LOG ( Sigaction() error. ) \n");

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
					read_routine(/*serial_fd,*/sock[i], buf, i);

			}
		}
	}

	for (i = 0; i < MAX_SERVER; i++)
		close(sock[i]);

	//serialClose(serial_fd);

	return 0;
}

void read_routine(/*int fd,*/int sock, unsigned char *buf, int servType) {

	int tempSec = getSec();
	int OESec = tempSec % 2;

	printf("connetion time -> sec : %d, odd-even : %d\n", tempSec, OESec);

	while (1) {
		int str_len = read(sock, buf, BUF_SIZE);
		if (str_len == 0)
			return;

		if (dataCheck(str_len, buf) == 1) {

			char path[256] = { 0, };

			switch (servType) {
			case 0:
				sprintf(path, "%s%4d_%2d_%2d_%2d_%2d_%2d.txt", ENGINEROOM,
						getYear(), getMonth(), getDay(), getHour(), getMin(),
						getSec());
				printf("log file is : %s\n", path);
				break;
			case 1:
				sprintf(path, "%s%4d_%2d_%2d_%2d_%2d_%2d.txt", BRIDGEROOM,
						getYear(), getMonth(), getDay(), getHour(), getMin(),
						getSec());
				break;
			}

			fileWrite(path, buf);

			if (fileRemove(servType)) {
				printf("file remove error\n");
			}

		}

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
