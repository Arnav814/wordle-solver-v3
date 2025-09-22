#include "fsutils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

time_t getMTime(const char* const path) {
	struct stat attr;
	int status = stat(path, &attr);

	if (status == -1) {
		printf("Failed to stat file %s, got error %i.\n", path, errno);
		exit(1);
	}

	return attr.st_mtim.tv_sec;
}

#define APPLICATION_DIR "/wordlebot3"

char* getXdgPath(const char* const envName, const char* const defaultPath) {
	char* baseDir = getenv(envName);

	// if $envName is set and not empty
	if (baseDir && *baseDir) {
		// the Spec says paths must be absolute, and It must be Obeyed
		if (baseDir[0] != '/') {
			printf("Paths specified for XDG_*_HOME variables must be absolute; "
					"%s=\"%s\" is not and will be ignored.\n", envName, baseDir);
			baseDir = NULL;
		} else {
			baseDir = strdup(baseDir);
		}
	}
	
	// if the baseDir wasn't in the environment variable or it was invalid
	if (baseDir == NULL) {
		const char* home = getenv("HOME");
		if (home == NULL || strcmp(home, "") == 0) {
			printf("Couldn't find user's home directory.\n");
			exit(1);
		}

		// fallback to $HOME/defaultPath
		baseDir = malloc(strlen(home) + strlen(defaultPath) + 1);
		strcpy(stpcpy(baseDir, home), defaultPath); // baseDir = home + defaultPath
	}

	baseDir = realloc(baseDir, strlen(baseDir) + strlen(APPLICATION_DIR) + 1);
	strcat(baseDir, APPLICATION_DIR);
	// in most cases, baseDir should now be $HOME/defaultPath/wordlebot3

	// if baseDir doesn't exist, create it
	if(mkdir(baseDir, 0775) < 0 && errno != EEXIST) {
		printf("Error %i when attempting to create directory %s.\n", errno, baseDir);
		exit(1);
	}

	return baseDir;
}

FILE* recursivelySearch(const char* const dirname, const char* const filename) {
	DIR* dir = opendir(dirname);
	if (!dir) {
		printf("Failed to open directory \"%s\" for searching (error %i).\n", dirname, errno);
		exit(1);
	}

	errno = 0;
	for (struct dirent* dirEntry = readdir(dir); dirEntry != NULL; dirEntry = readdir(dir)) {
		if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) continue;

		char* entryPath = malloc(strlen(dirname) + strlen(dirEntry->d_name) + 2);
		strcpy(entryPath, dirname);
		strcat(entryPath, "/");
		strcat(entryPath, dirEntry->d_name);
		
		printf("Opening %s\n", entryPath);

		DIR* subdir = opendir(entryPath);

		if (subdir) { // this is a directory and it was succesfully opened
			closedir(subdir);

			// do the recursion
			FILE* file = recursivelySearch(entryPath, filename);

			free(entryPath);
			if (file) {
				closedir(dir);
				return file;
			}

		} else if (errno == ENOTDIR) { // this was actually a file
			if (strcmp(filename, dirEntry->d_name) == 0) { // FIXME
				FILE* file = fopen(entryPath, "r");
				if (!file) {
					printf("Failed to open file \"%s\" for reading (error %i).\n", entryPath, errno);
					exit(1);
				}

				closedir(dir);
				free(entryPath);
				return file;
			}
			free(entryPath);

		} else { // an actual error occured
			printf("Failed to open file or directory \"%s\" (error %i).\n", entryPath, errno);
			exit(1);
		}
		errno = 0;
	}

	if (errno) {
		printf("An error occured when searching for %s (errno: %i).\n", filename, errno);
		exit(1);
	}

	closedir(dir);
	return NULL;
}

