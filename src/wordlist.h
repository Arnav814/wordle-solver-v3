#ifndef WORDLIST_H
#define WORDLIST_H
#include "config.h"
#include "pattern.h"

typedef struct {
	uint count;
	Pattern* data;
} Wordlist;

// allocates memory automatically
Wordlist loadWordlist(char* filePath);

void printWords(const Wordlist words);

// returns a copy of the wordlist, allocating memory by itself
Wordlist copyWordlist(const Wordlist initial);

bool wordlistHasValue(const Wordlist wordlist);

bool wordlistsAreEqual(const Wordlist a, const Wordlist b);

#endif /* WORDLIST_H */
