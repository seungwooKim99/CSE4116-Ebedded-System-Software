#include "default.h"
#define HASHED_PW_FILE "/sdcard/hashed-password"
#define ENTER_PASSWORD _IOR(DEVICE_MAJOR_NUMBER, 0, int *)
#define HASH_SIZE 100
int hash(int number);

extern bool passwordExist();
extern int enterAndGetPassword();
extern bool savePassword(int password);
extern bool comparePassword(int password);

/* check if hashed-password file exists */
bool passwordExist() {
	FILE *fp = fopen(HASHED_PW_FILE, "r");
	bool exist = (fp) ? true : false;
	if(fp) {
		fclose(fp);
	}
	return exist;
}

int enterAndGetPassword() {
	int fd = open(DEVICE_NAME_PATH, O_RDWR);
	int pw[1];
	ioctl(fd, ENTER_PASSWORD, pw);
	LOGV("ioctl done : %d", pw[0]);
	close(fd);
	return pw[0];
}

bool savePassword(int password) {
	FILE *fp = fopen(HASHED_PW_FILE, "w");
	bool result = (fp) ? true : false;
	if(!fp){
		return result;
	}
	int hashed_pw = hash(password);
	char hashed_buf[HASH_SIZE];
	sprintf(hashed_buf, "%d", hashed_pw);
	fprintf(fp, "%s", hashed_buf);
	fclose(fp);
	return result;
}

bool comparePassword(int password) {
	FILE *fp = fopen(HASHED_PW_FILE, "r");
	bool result = (fp) ? true : false;
	if(!fp){
		return result;
	}
	int hashed_pw = hash(password);
	char hashed_buf[HASH_SIZE];
	sprintf(hashed_buf, "%d", hashed_pw);

	/* compare password */
	char saved_pw_buf[HASH_SIZE];
	fgets(saved_pw_buf, sizeof(saved_pw_buf), fp);
	LOGV("input pw:%s\n", hashed_buf);
	LOGV("saved pw:%s\n", saved_pw_buf);
	fclose(fp);
	return (strcmp(saved_pw_buf, hashed_buf) == 0) ? true : false;
}

/* for hashing password */
int hash(int number) {
	// have to modify!
	return number;
}
