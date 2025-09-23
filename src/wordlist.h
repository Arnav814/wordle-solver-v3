#ifndef WORDLIST_H
#define WORDLIST_H
#include "cliparse.h"
#include "pattern.h"
#include <stdio.h>

typedef struct {
	uint count;
	Pattern* data;
} Wordlist;

// turn a wordlist name (ex: "small.txt") into an absolute path (ex: "/usr/share/wordlebot3/small.txt")
// returns a pointer to the open file or crashes if the file can't be found
FILE* lookupWordlist(const char* const path, const Config* const config);

// allocates memory automatically
Wordlist loadWordlist(FILE* file);

void printWords(const Wordlist words);

// returns a copy of the wordlist, allocating memory by itself
Wordlist copyWordlist(const Wordlist initial);

bool wordlistHasValue(const Wordlist wordlist);

bool wordlistsAreEqual(const Wordlist a, const Wordlist b);

#endif /* WORDLIST_H */
