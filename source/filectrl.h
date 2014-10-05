/*
 ============================================================================
 Created on	:	2014. 9. 30.
 Name			:	filectrl.h
 Author		:	JeongHakOh
 Version		:
 Copyright		:	JeongHakOh
 Description	:	C Language based filectrl, Ansi-style
 URL			:	https://github.com/RasTSC/RasC
 ============================================================================
 */

#ifndef FILECTRL_H_
#define FILECTRL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#define ROOT "log"
#define ENGINE "log/engine"
#define BRIDGE "log/bridge"
#define ENGINELOG "log//engine"
#define BRIDGELOG "log//bridge"

extern unsigned char *nonHeader(unsigned char *str);
extern int fileWrite(char *path, unsigned char *buf);
extern int fileRemove(int servType);

#endif /* FILECTRL_H_ */
