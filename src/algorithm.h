#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "pattern.h"
#include "wordlist.h"

// returns a copy of the provided wordlist containing all words that match pattern
Wordlist filter(const Pattern pattern, const Wordlist words) {
	// allocate space assuming all words pass; probably too much space, but who cares
	Pattern* outBuf = calloc(words.count, sizeof(Pattern));
	uint destIdx = 0;
	for (uint srcIdx = 0; srcIdx < words.count; srcIdx++) {
		if (checkPattern(pattern, words.data[srcIdx])) {
			memcpy(&outBuf[destIdx], &words.data[srcIdx], sizeof(Pattern));
			destIdx++;
		}
	}

	// shrink buffer to correct size; no +1 because destIdx is incremented after the copy
	outBuf = reallocarray(outBuf, destIdx, sizeof(Pattern));
	Wordlist out = {destIdx, outBuf};
	return out;
}

// count how many words from words pass pattern, but don't actually create the output
uint countFilter(const Pattern pattern, const Wordlist words) {
	uint matching = 0;
	for (uint i = 0; i < words.count; i++) {
		if (checkPattern(pattern, words.data[i])) {
			matching++;
		}
	}
	return matching;
}

#endif /* ALGORITHM_H */
