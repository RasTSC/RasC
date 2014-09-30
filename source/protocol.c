/*
 ============================================================================
 Created on	: 2014. 9. 25.
 Name			: protocol.c
 Author		: JeongHakOh
 Version		:
 Copyright		: JeongHakOh
 Description	: C Language based Protocol, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include "protocol.h"

//////////////////////////////////////////////////////////////////////////////////////
//							dataCheck() Definition								//
//						: Protocol Data Check Sum								//
//							(received message)									//
//////////////////////////////////////////////////////////////////////////////////////
int dataCheck(int readSize, unsigned char *rs232c_buf) {

	unsigned char checksum = 0;
	int index = 0;

	printf("readSize = %d\n", readSize);
	if (readSize <= 0 || readSize > INBUF_MAX) {
		return 0;
	}

	while (index < readSize - 2) {
		if (c_state == IDLE) {
			c_state =
					(rs232c_buf[0] == '$' || rs232c_buf[0] == '#') ?
							PAYLOAD : IDLE;
			index++;
		} else if (c_state == PAYLOAD) {

			printf("add msp_buf[%d] = %c, checksum = (%%c)%c, (%%x)%x\n", index,
					rs232c_buf[index], checksum, checksum);
			checksum ^= (rs232c_buf[index++] & 0xFF);

			c_state = (rs232c_buf[index] == '*') ? CHECKSTAR : PAYLOAD;

		} else if (c_state == CHECKSTAR) {
			index++;
			printf("index(%d)\n", index);
		}
	}

	if (rs232c_buf[index + 1] == checksum) {
		printf("data true... checksum = %x\n", rs232c_buf[index + 1]);
		c_state = IDLE;
		return 1;
	} else if (c_state == IDLE) {
		printf("not header '$' | '#'\n");
		return 0;
	} else {
		printf("not checksum = %x\n", rs232c_buf[index]);
		c_state = IDLE;
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//							evaluateCommand() Definition						//
//			: evaluate Command for RS232C Serial Protocol Data				//
//					(received message parsing)									//
//////////////////////////////////////////////////////////////////////////////////////
//
//void evaluateCommand(unsigned char cmd, int dataSize)
//{
//  int i;
//  int icmd = (int)(cmd & 0xFF);
//
//  switch (icmd)
//  {
//  case :
//
//  default:
//    printf("ERROR : Don't know how to handle reply -> ( %d )", cmd);
//    break;
//  }
//}
//////////////////////////////////////////////////////////////////////////////////////
