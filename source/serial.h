/*
 ============================================================================
 Created on  : 2014. 9. 22.
 Name        : serial.h
 Author      : JeongHakOh
 Version     :
 Copyright   : JeongHakOh
 Description : C Language based serial.h, Ansi-style
 ============================================================================
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <termios.h>
#include <termio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////
//                           Serial settings define                      //
//////////////////////////////////////////////////////////////////////////////////////

//  B300, B1200, B4800, B9600, B19200, B38400, B57600, B115200
#define BAUDRATE B4800
#define MODEDEVICE "/dev/ttyAMA0"
#define DATA_MAX 20
#define SBUF_SIZE 255
//////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
//                                function define                        //
//////////////////////////////////////////////////////////////////////////////////////

// serial open (option settings), serial close
extern int serialOpen();
extern void serialClose(int fd);

//file data read write
extern void sendFileData(int fd, char *filename);
extern void readFileData(int fd, char *filename);

// msg data read write
extern void serialWrite(int fd, char *msg_buf);
extern void serialRead(int fd, char *msg_buf);
//////////////////////////////////////////////////////////////////////////////////////

#endif /* SERIAL_H_ */
