#ifndef CLIPARSE_H
#define CLIPARSE_H
#include <sys/types.h>

typedef struct {
	char* wordsFile; // file path to load wordlist from
	char* solutionsFile; // file path to load solutions from
	uint jobs; // number of threads
	uint verbosity; // verbosity, 0 to 3, default 1
} Config;

// argv is modified and unusable after this; make a copy if needed
Config* configParse(const int argc, char** argv);

// delete a config object
void configFree(Config* config);

#endif /* CLIPARSE_H */
