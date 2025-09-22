#ifndef FSUTILS_H
#define FSUTILS_H

#include <dirent.h>
#include <stdio.h>
#include <time.h>

// gets the modification time of the file at path in seconds, crashing if the file doesn't exist
time_t getMTime(const char* const path);

// gets the path for an XDG base dir; uses envName as the name of the appropriate environment
// variable (ex: XDG_CACHE_HOME) and defaultPath as the fallback, relative to $HOME, (ex: ".config")
// also creates the directory if necessary and append the name of the application to it
// spec is here: https://specifications.freedesktop.org/basedir-spec/latest/
char* getXdgPath(const char* const envName, const char* const defaultPath);

// recursively search dir for filename, returning null if nothing is found, otherwise returns file
// opened with read-only access
FILE* recursivelySearch(const char* const dirname, const char* const filename);

#endif /* FSUTILS_H */
