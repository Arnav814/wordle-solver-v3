#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "map.h"
#include "pattern.h"
#include "wordlist.h"

// returns a copy of the provided wordlist containing all words that match pattern
Wordlist filter(const Pattern pattern, const Wordlist words);

// count how many words from words pass pattern, but don't actually create the output
uint countFilter(const Pattern pattern, const Wordlist words);

// simulate the result of guessing `guess`, provided `solution` is the solution
Pattern simGuess(const Pattern guess, Pattern solution);

// for every possible solution, pretend to guess `guess`, then count how many
// possible solutions are left and sum the result; takes in a hashmap to cache patterns counted,
// modifies the cache, needs keys to be freed later
ulong cumulativeWordsLeft(const Pattern guess, const Wordlist solutions, hashmap* const cache);

// find the best word to guess, where allWords is the full wordlist and
// solutions is all possible solutions
// also displays a progress bar
Pattern findBestWord(const Wordlist allWords, const Wordlist solutions);

#endif /* ALGORITHM_H */
