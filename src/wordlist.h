#ifndef WORDLIST_H
#define WORDLIST_H
#include "pattern.h"

typedef struct {
	uint count;
	Pattern* data;
} Wordlist;

// allocates memory automatically
Wordlist loadWordlist(const char* const path);

void printWords(const Wordlist words);

// returns a copy of the wordlist, allocating memory by itself
Wordlist copyWordlist(const Wordlist initial);

#endif /* WORDLIST_H */
