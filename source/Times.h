/*
 ============================================================================
 Created on  : 2014. 9. 21.
 Name        : Times.h
 Author      : JeongHakOh
 Version     :
 Copyright   : JeongHakOh
 Description : C Language based Times.h, Ansi-style
 ============================================================================
 */

#ifndef TIMES_H_
#define TIMES_H_

#include <stdio.h>
#include <time.h>

typedef struct _offsettime{
	int y;
	int mon;
	int d;
	int h;
	int min;
	int s;
}ostime;

int getYear() {
	time_t sysTime;
	struct tm *tmp;
	sysTime = time(NULL);
	tmp = localtime(&sysTime);

	return tmp->tm_year + 1900;
}

int getMonth() {
	time_t sysTime;
	struct tm *tmp;
	sysTime = time(NULL);
	tmp = localtime(&sysTime);

	return tmp->tm_mon + 1;
}

int getDay() {
	time_t sysTime;
	struct tm *tmp;
	sysTime = time(NULL);
	tmp = localtime(&sysTime);

	return tmp->tm_mday;
}

int getHour() {
	time_t sysTime;
	struct tm *tmp;
	sysTime = time(NULL);
	tmp = localtime(&sysTime);

	return tmp->tm_hour;
}

int getMin() {
	time_t sysTime;
	struct tm *tmp;
	sysTime = time(NULL);
	tmp = localtime(&sysTime);

	return tmp->tm_min;
}

int getSec() {
	time_t sysTime;
	struct tm *tmp;
	sysTime = time(NULL);
	tmp = localtime(&sysTime);

	return tmp->tm_sec;
}

ostime pointTime() {
	ostime pt;
	pt.y = getYear();
	pt.mon = getMonth();
	pt.d = getDay();
	pt.h = getHour();
	pt.min = getMin();
	pt.s = getSec();
	return pt;
}

#endif /* TIMES_H_ */
