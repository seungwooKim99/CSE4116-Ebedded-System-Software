#include "default.h"

extern bool passwordExist();
extern int enterAndGetPassword();
extern bool savePassword(int password);
extern bool comparePassword(int password);
extern bool startSession(int sec);

jboolean JNICALL Java_com_example_filemanager_MainActivity_passwordExists(JNIEnv *env, jobject thisObj){
	return passwordExist();
}


jint JNICALL Java_com_example_filemanager_PasswordService_enterAndGetPassword(JNIEnv *env, jobject thisObj){
	return enterAndGetPassword();
}

jboolean JNICALL Java_com_example_filemanager_PasswordService_savePassword(JNIEnv *env, jobject thisObj){
	jclass thisClass = (*env)->GetObjectClass(env, thisObj);
	jfieldID fidnumber = (*env)->GetFieldID(env, thisClass, "curr_password", "I");
	jint password = (*env)->GetIntField(env, thisObj, fidnumber);

	//save password
	return savePassword(password);
}

jboolean JNICALL Java_com_example_filemanager_PasswordService_comparePassword(JNIEnv *env, jobject thisObj){
	jclass thisClass = (*env)->GetObjectClass(env, thisObj);
	jfieldID fidnumber = (*env)->GetFieldID(env, thisClass, "curr_password", "I");
	jint password = (*env)->GetIntField(env, thisObj, fidnumber);

	//compare password
	return comparePassword(password);
}

jobject JNICALL Java_com_example_filemanager_FileListActivity_fetchFiles(JNIEnv *env, jobject thisObj, jstring j_cwd_) {
	jclass fileListActivityCls = (*env)->GetObjectClass(env, thisObj); //get class

	/* Create ArrayList<File> instance */
	jclass arrayListCls = (*env)->FindClass(env, "java/util/ArrayList");
	jmethodID arrayListConstructor = (*env)->GetMethodID(env, arrayListCls, "<init>", "()V");
	jobject files = (*env)->NewObject(env, arrayListCls, arrayListConstructor);

	/* Add File elements */
	jclass fileCls = (*env)->FindClass(env, "com/example/filemanager/File");
	jmethodID fileConstructor = (*env)->GetMethodID(env, fileCls, "<init>", "(Ljava/lang/String;Ljava/lang/String;ZJZZZ)V");

	/* get files */
	const char *cwd = (*env)->GetStringUTFChars(env, j_cwd_, 0);
	DIR *dir = NULL;
	struct dirent *entry = NULL;

	if((dir = opendir(cwd)) == NULL){
		LOGV("current directory error\n");
		exit(1);
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { // skip . and ..
			continue;
		 }
		char path[1024];
		sprintf(path, "%s/%s", cwd, entry->d_name);

		jstring j_name = (*env)->NewStringUTF(env, entry->d_name); //name
		jstring j_cwd = (*env)->NewStringUTF(env, cwd); //cwd

		struct stat fileStat;
		if (lstat(path, &fileStat) == -1) {
			LOGV("Something's wrong!!!");
		    continue;
		}

		jboolean j_isDir, j_R_OTH, j_W_OTH, j_X_OTH;
		if (S_ISLNK(fileStat.st_mode)) {
			/* If Symbolic link */
			struct stat linkStat;
			if (stat(path, &linkStat) == -1) continue;
			j_isDir = S_ISDIR(linkStat.st_mode);
			j_R_OTH = (access(path, R_OK) == 0);
			j_W_OTH = (access(path, W_OK) == 0);
			j_X_OTH = (access(path, X_OK) == 0);
		} else {
			j_isDir = S_ISDIR(fileStat.st_mode);
			j_R_OTH = fileStat.st_mode & S_IROTH; //other's read permission
			j_W_OTH = fileStat.st_mode & S_IWOTH; //other's write permission
			j_X_OTH = fileStat.st_mode & S_IXOTH; //other's execute permission
		}

		jlong j_size = fileStat.st_size; //size
		jobject file = (*env)->NewObject(env, fileCls, fileConstructor, j_name, j_cwd, j_isDir, j_size, j_R_OTH, j_W_OTH, j_X_OTH);
        (*env)->CallBooleanMethod(env, files, (*env)->GetMethodID(env, arrayListCls, "add", "(Ljava/lang/Object;)Z"), file);

        (*env)->DeleteLocalRef(env, j_name);
        (*env)->DeleteLocalRef(env, j_cwd);
        (*env)->DeleteLocalRef(env, file);
	}
	(*env)->ReleaseStringUTFChars(env, j_cwd_, cwd);
	(*env)->DeleteLocalRef(env, fileListActivityCls);
	(*env)->DeleteLocalRef(env, arrayListCls);
	(*env)->DeleteLocalRef(env, fileCls);
	return files;
}

jboolean JNICALL Java_com_example_filemanager_FileListActivity_canOpenDir(JNIEnv *env, jobject thisObj, jstring jpath) {
	const char *path = (*env)->GetStringUTFChars(env, jpath, 0);
	DIR *dir = NULL;
	if((dir = opendir(path)) == NULL){
		return false;
	}
	closedir(dir);
	return true;
}

jboolean JNICALL Java_com_example_filemanager_FileListActivity_deleteFile(JNIEnv *env, jobject thisObj, jstring filePath){
	const char *file_path = (*env)->GetStringUTFChars(env, filePath, NULL);
	int result = remove(file_path);
    (*env)->ReleaseStringUTFChars(env, filePath, file_path);
    return (result == 0) ? true : false;
}

jboolean JNICALL Java_com_example_filemanager_FileListActivity_startSession(JNIEnv *env, jobject thisObj, jint sec){
	return startSession(sec);
}

jstring JNICALL Java_com_example_filemanager_FileActivity_readFile(JNIEnv *env, jobject thisObj, jstring filePath){
	const char* path = (*env)->GetStringUTFChars(env, filePath, 0);
	FILE* file = fopen(path, "r");
	if (file == NULL) {
	    return NULL;
	}

	struct stat st;
	if (stat(path, &st) == -1) {
	    fclose(file);
	    return NULL;
	}
	long size = st.st_size;

	char* buffer = (char*) malloc(size + 1);
	if (buffer == NULL) {
	    fclose(file);
	    return NULL;
	}

	size_t bytesRead = fread(buffer, sizeof(char), size, file);
	buffer[bytesRead] = '\0';

	fclose(file);

	jstring result = (*env)->NewStringUTF(env, buffer);
	free(buffer);
	(*env)->ReleaseStringUTFChars(env, filePath, path);

	return result;
}

jboolean JNICALL Java_com_example_filemanager_FileActivity_editFile(JNIEnv *env, jobject thisObj, jstring filePath, jstring body){
	const char *file_path = (*env)->GetStringUTFChars(env, filePath, NULL);
	const char *file_body = (*env)->GetStringUTFChars(env, body, NULL);

	FILE *file = fopen(file_path, "w");
	if (file == NULL) {
	    return false;
	}
    fputs(file_body, file);
    fclose(file);
    (*env)->ReleaseStringUTFChars(env, filePath, file_path);
    (*env)->ReleaseStringUTFChars(env, body, file_body);
    return true;
}
