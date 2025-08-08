#include "userio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Pattern parsePattern(const Pattern word, const char* const text) {
	// bad, placeholder error handling
	for (uint i = 0; i < 5; i++) {
		assert(text[i] == 'g' || text[i] == 'y' || text[i] == 'n');
	}

	Pattern out = ANYTHING;
	// have to check before, to remove from the entire pattern without affecting duplicates
	// Duplicates that are in the guess but not the word will have the first
	// instance be yellow, and any subsequent instances be gray. Don't remove those.
	for (int i = 4; i >= 0; i--) {
		if (text[i] == 'n') {
			for (uint k = 0; k < 5; k++) {
				out.data[k] &= ~word.data[i]; // remove word[i] from the entire pattern
			}
		} else if (text[i] == 'y') {
			for (uint k = 0; k < 5; k++) {
				out.data[k] |= word.data[i]; // add word[i] back, in case we removed it
			}
		}
	}

	for (uint i = 0; i < 5; i++) {
		if (text[i] == 'g')
			out.data[i] = word.data[i]; // must be exact
		else if (text[i] == 'y')
			// anything but the letter, TODO: handle better
			out.data[i] &= ~word.data[i];
	}

	return out;
}

Pattern readPattern(const Pattern word) {
	char wordStr[6];
	wordStr[5] = 0;
	pattern2str(word, wordStr);
	printf("Guessed: %s\n", wordStr);

	char* input;
	scanf("%ms", &input);
	assert(strlen(input) >= 5); // TODO: better error handling
	Pattern out = parsePattern(word, input);
	free(input);
	return out;
}
