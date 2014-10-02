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

#include "filectrl.h"

unsigned char *nonHeader(unsigned char *str) {
	return &str[1];
}

int fileWrite(char *path, unsigned char *buf) {

	FILE *fp = fopen(path, "w+");

	if (fp == NULL) {
		printf("fopen() ERROR\n");

		return 0;
	}

	unsigned char *tmpstr = nonHeader(buf);
//	fprintf(fp, "buf : %s", buf);
	fprintf(fp, "tmp : %s", tmpstr);

	fclose(fp);

	return 1;
}

int fileRemove(int servType) {

	DIR *pdir;
	struct dirent *pde;
	struct stat buf;

	int i = 0;
	int count = 0;
	char *dir_name[255];
	char pwd[1024] = { 0, };
	char *temp = NULL;

	memset(dir_name, '\0', sizeof(dir_name));
	memset(&pde, '\0', sizeof(pde));
	memset(&buf, '\0', sizeof(buf));

	// current work dir....
	switch (servType) {
	case 0:
		chdir(ENGINELOG);
		break;
	case 1:
		chdir(BRIDGELOG);
		break;
	}

	getcwd(pwd, 1024);

	if ((pdir = opendir(pwd)) < 0) {
		perror("opendir");
		return 1;
	} else {

		printf("\n\n\t%s\n\n", pwd);

		while ((pde = readdir(pdir)) != NULL) {
			lstat(pde->d_name, &buf);

			if (S_ISDIR(buf.st_mode) == 0) {
				if (!strcmp(pde->d_name, ".") || !strcmp(pde->d_name, "..")) {
					;
				} else {
					dir_name[count] = pde->d_name;
					count++;
				}
			}
		}

		if (count > 3) {

			temp = dir_name[0];

			for (i = 1; i < count; i++) {

				if (strcmp(temp, dir_name[i]) > 0)
					temp = dir_name[i];
			}

			printf("\nremove file : %s\n", temp);
			remove(temp);

		}

//		printf("file is : ");
//		for (i = 0; i < count - 1; i++) {
////		printf("%s\t", dir_name[i]);
//
//			if (strcmp(dir_name[i], dir_name[i + 1]) < 0)
//				printf("%s\t", dir_name[i]);
//
//		}
//		printf("\n");

		closedir(pdir);

		chdir("..//..");
	}

	return 0;
}
