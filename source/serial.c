/*
 ============================================================================
 Created on  : 2014. 9. 22.
 Name        : serial.c
 Author      : JeongHakOh
 Version     :
 Copyright   : JeongHakOh
 Description : C Language based serial.c, Ansi-style
 ============================================================================
 */

#include "serial.h"

//////////////////////////////////////////////////////////////////////////////////////
//                                function define                        //
//////////////////////////////////////////////////////////////////////////////////////

// serial open (option settings), serial close
int serialOpen();
void serialClose(int fd);

//file data read write
void sendFileData(int fd, char *path, char *filename, int dataType);
void readFileData(int fd);

// msg data read write
void serialWrite(int fd, char *msg_buf);
void serialRead(int fd, char *msg_buf);
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//                                  Serial Open                          //
//////////////////////////////////////////////////////////////////////////////////////

int serialOpen() {
	struct termios /*oldtio, */newtio;

	int fd = open(MODEDEVICE, O_RDWR | O_NOCTTY /*| O_SYNC*/);
	//O_RDWR : read & write 모드로 file open
	//O_NOCTTY : controlling tty가 안도도록 한다.
	//       오픈되어있는 장치가 컨트롤링 터미널이 되지 못하도록 함
	//       컨트롤링 터미널 : 디바이스가 현재 터미널을 제어함을 의미

	if (fd < 0) {
		printf("Serial %s Device Err\n", MODEDEVICE);
		exit(-1);
	}

	printf("Connect Success Serial Port : %s \n", MODEDEVICE);

	//tcgetattr(fd, &oldtio);
	//oldtio가 가리키는 구조체 내에 터미널 인터페이스 변수의 현재값을 저장
	bzero(&newtio, sizeof(newtio));    //바이트스트링 newtio의 첫 바이트를 0
	//memset(&newtio, 0, sizeof(newtio));

	newtio.c_iflag = /*IGNPAR*/ICRNL;   //입력관련 터미널 속성을 변경 처리
	newtio.c_oflag = 0;     //출력관련 속성변경 처리
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;   //제어관련
	//BAUDRATE : 전송속도, CS8 : 8n1(8bit no parity, 1stopbit)
	//CLOCAL : local connection 모뎀제어를 안함
	//CREAD : 문자수신가능
	newtio.c_lflag = 0;     //실제 보여지는 터미널의 속성을 제어
	//newtio.c_cc[VTIME] = 0;       //제어문자를 제어
	//newtio.c_cc[VMIN] = 1;    //read시 리턴되기 위한 최소문자 개수

	tcflush(fd, TCIFLUSH);      //시리얼 포트 수신 큐 초기화

	if (tcsetattr(fd, TCSANOW, &newtio) < 0)  //시리얼 포트에 새 속성 적용
			{
		perror("init_serialport: Couldn't set term attributes\n");
		return -1;
	}

	//usleep(6000000);

	return fd;
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//                                  Serial Close                         //
//////////////////////////////////////////////////////////////////////////////////////

void serialClose(int fd) {
	close(fd);
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//                                  Send File Data                       //
//////////////////////////////////////////////////////////////////////////////////////

void sendFileData(int fd, char *path, char *filename, int dataType) {

	FILE *fp = fopen(path, "rb");
	if (fp == NULL) {
		printf("sendFileData file open error\n");
		exit(0);
	}

	unsigned char *buf;
	char sHeader[256] = { 0, };
	char *end = "!";

	sprintf(sHeader, "#@%s", filename);

	fseek(fp, 0, SEEK_END);
	long int FileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = (unsigned char*) malloc(FileSize);

	fread(buf, FileSize, 1, fp);
	fclose(fp);

	printf("fread : %s\n", buf);

	if (write(fd, sHeader, strlen(sHeader)) < 0) {
		printf("Serial header writing Error\n");
		exit(0);
	}
	while (write(fd, buf, FileSize) < 0) {
		printf("writing Error\n");
		exit(0);
	}

	if (write(fd, end, strlen(end)) < 0) {
		printf("file end writing Error\n");
		exit(0);
	}

	printf("serial file send sucess\n");

	free(buf);
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//                                  Read File Data                       //
//////////////////////////////////////////////////////////////////////////////////////

void readFileData(int fd) {

	int dataType = 0;
	unsigned char buf[1024] = { 0, };
	char path[255] = { 0, };
	char filename[256] = { 0, };
	char *p;

	if (read(fd, buf, 1024) > 0) {

		p = strtok(buf, "@");
		if (strcmp(p, "#") == 0) {
			sprintf(filename, "%s", p);
			p = strtok(filename, "_");
			if (strcmp(p, "e") == 0) {
				dataType = 0;
				sprintf(path, "%s/%s", ENGINE, filename);
			} else if (strcmp(p, "b") == 0) {
				dataType = 1;
				sprintf(path, "%s/%s", BRIDGE, filename);
			}
		}

		FILE *fp = fopen(path, "wb");
		if (fp == NULL) {
			printf("file open error\n");
			exit(0);
		}

		//fprintf(stdout, "%s", buf);
		//fprintf(fp, "%s", buf);

		while (read(fd, buf, 1) > 0) {
			//fprintf(stdout, "%s", buf);
			fprintf(fp, "%s", buf);
			if (buf != NULL)
				fprintf(fp, "%s", buf);
		}

		printf("fclose()\n");
		fclose(fp);

		printf("dataType = %d\n", dataType);
//		if (fileRemove(dataType)) {
//			printf("file remove error\n");
//		}
	}
	tcdrain(fd);
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//                                Serial msg write                       //
//////////////////////////////////////////////////////////////////////////////////////

void serialWrite(int fd, char *msg_buf) {
	if (write(fd, msg_buf, strlen(msg_buf) + 1) < 0) {
		printf("writing Error\n");
		exit(0);
	}

	tcdrain(fd);
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//                                Serial msg read                        //
//////////////////////////////////////////////////////////////////////////////////////

void serialRead(int fd, char *msg_buf) {
	int read_cnt = read(fd, msg_buf, SBUF_SIZE);

	if (read_cnt > 0)
		printf("read : %s\n", msg_buf);

	tcdrain(fd);
}
//////////////////////////////////////////////////////////////////////////////////////

