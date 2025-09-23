#ifndef CLIPARSE_H
#define CLIPARSE_H
#include <sys/types.h>

typedef struct {
	char* wordsFile; // file path to load wordlist from
	char* solutionsFile; // file path to load solutions from

	char** searchPath; // places to search for wordlists before directly looking up a path,
					   // read from environment variable $WORDLIST_PATH
	uint searchEntries; // length of searchpath

	uint jobs; // number of threads
	uint verbosity; // verbosity, 0 to 3, default 1

	char* solution; // run automatically, counting guesses to finding a provided solution.
					// constains the actual solution if autosolve is specified otherwise null

} Config;


// turn a wordlist name (ex: "small.txt") into an absolute path (ex: "/usr/share/wordlebot3/small.txt")
// returns the filename or crashes if the file can't be found
char* lookupWordlist(const char* const path, const Config* const config);

// argv is modified and unusable after this; make a copy if needed
Config* configParse(const int argc, char** argv);

// delete a config object
void configFree(Config* config);

#endif /* CLIPARSE_H */
