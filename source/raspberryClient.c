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
#define INITFILE "init.txt"
#define PORT "1234"
//#define ENGINEIP "127.0.0.1"
//#define BRIDGEIP "127.0.0.1"
#define ENGINEROOM "log/engine/e_"
#define BRIDGEROOM "log/bridge/b_"

int cntable[MAX_SERVER] = { 0, };

void read_routine(/*int fd,*/int sock, unsigned char *buf, int servType);
void ctrl_childproc(int sig);
void error_handling(char *message);

int main(int argc, char *argv[]) {
	int sock[MAX_SERVER];
	char *iptable[MAX_SERVER] = { 0, }; //{ ENGINEIP, BRIDGEIP };
	int i = 0;

	pid_t pid;
	struct sigaction act;
	unsigned char buf[BUF_SIZE] = { 0, };
	struct sockaddr_in server_addr[MAX_SERVER];

	FILE *fp = fopen(INITFILE, "r");
	char iptemp[255];
	char *p;
	while (!feof(fp))  // 파일의 끝이 아니라면
	{
		fgets(iptemp, 255, fp);  // 최대 80칸짜리 한줄 읽기

		p = strtok(iptemp, ":");
		if (strcmp(iptemp, "e") == 0) {
			p = strtok(NULL, " ");
			if (p == NULL)
				break;
			else
				iptable[0] = p;
			//printf("iptable[0] = %s\n", iptable[0]);
		} else if (strcmp(iptemp, "b") == 0) {
			p = strtok(NULL, " ");
			if (p == NULL)
				break;
			else
				iptable[1] = p;
			//printf("iptable[1] = %s\n", iptable[1]);
		}
	}

	fclose(fp);

//	if (argc != ((2 * MAX_SERVER) + 1)) {
//		printf("Usage : %s <IP1> <port> <IP2> <port>\n", argv[0]);
//		exit(1);
//	}

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
		server_addr[i].sin_addr.s_addr = inet_addr(iptable[i]);	//argv[((2 * i) + 1)]);
		server_addr[i].sin_port = htons(atoi(PORT));	//argv[((2 * i) + 2)]);
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

	ostime pt, ct;
	pt = pointTime();

	printf("read_routine %04d_%02d_%02d_%02d_%02d_%02d\n", pt.y, pt.mon, pt.d,
			pt.h, pt.min, pt.s);

	int tempSec = getSec();
	int OESec = tempSec % 2;

	printf("connetion time -> sec : %d, odd-even : %d\n", tempSec, OESec);

	while (1) {
		int str_len = read(sock, buf, BUF_SIZE);
		if (str_len == 0)
			return;

		// 나중에 여러개의 프로토콜이 들어왔을 경우 체크섬 하는 내용 datacheck 함수에 추가하기...
		if (dataCheck(str_len, buf) == 1) {

			char path[256] = { 0, };

			ct = pointTime();

			switch (servType) {
			case 0:
				sprintf(path, "%s%04d_%02d_%02d_%02d_%02d_%02d.txt", ENGINEROOM,
						ct.y, ct.mon, ct.d, ct.h, ct.min, ct.s);
				printf("log file is : %s\n", path);
				break;
			case 1:
				sprintf(path, "%s%04d_%02d_%02d_%02d_%02d_%02d.txt", BRIDGEROOM,
						ct.y, ct.mon, ct.d, ct.h, ct.min, ct.s);
				break;
			}

			fileWrite(path, buf);

			if(((pt.h+1)%24) == ct.h)
			{
				//sendFileData(sfd, path);
				pt = ct;
			}

			if (fileRemove(servType)) {
				printf("file remove error\n");
			}

		}

		buf[str_len] = 0;
		printf("Message from server: %s\n", buf);
		//serialWrite(sfd, buf);
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
