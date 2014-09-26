/*
  ============================================================================
 Created on	: 2014. 9. 25.
 Name			: protocol.h
 Author		: JeongHakOh
 Version		:
 Copyright		: JeongHakOh
 Description	: C Language based Protocol, Ansi-style
 ============================================================================
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define INBUF_MAX 1024

//////////////////////////////////////////////////////////////////////////////////////
//						Variables for Read Data									//
//////////////////////////////////////////////////////////////////////////////////////

static enum _serial_state{
	HEADER_ERR = -1,
	IDLE,
	PAYLOAD
}c_state = IDLE;

//////////////////////////////////////////////////////////////////////////////////////

extern int dataCheck(int readSize, unsigned char *rs232c_buf);
// protocol data parsing
//extern void evaluateCommand(unsigned char cmd, int dataSize);


#endif /* PROTOCOL_H_ */
