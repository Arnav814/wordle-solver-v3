#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "pattern.h"
#include "wordlist.h"

// returns a copy of the provided wordlist containing all words that match pattern
Wordlist filter(const Pattern pattern, const Wordlist words);

// count how many words from words pass pattern, but don't actually create the output
uint countFilter(const Pattern pattern, const Wordlist words);

// simulate the result of guessing `guess`, provided `solution` is the solution
Pattern simGuess(const Pattern guess, Pattern solution);

// for every possible solution, pretend to guess `guess`, then count how many
// possible solutions are left and sum the result
ulong cumulativeWordsLeft(const Pattern guess, const Wordlist solutions);

// contains the word and its score
typedef struct {
	Pattern word;
	ulong score;
} BestWord;

BestWord threadedFindWord(const Wordlist allWords, const Wordlist solutions, const uint threadCount);

#endif /* ALGORITHM_H */
