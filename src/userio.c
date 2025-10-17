#include "userio.h"
#include "algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Does *not* assume text is null-terminated */
bool basicValidate(const char* const text) {
	for (uint i = 0; i < 5; i++) {
		// the == 0 check will also catch input that is too short, provided the text is null terminated
		if (!(text[i] == 'g' || text[i] == 'y' || text[i] == 'n') || text[i] == 0) {
			return false;
		}
	}

	return true;
}

/* Assumes text is null terminated */
bool validateInput(const char* const text) {
	// needs null termination to check for extra chars
	return basicValidate(text) && strlen(text) == 5;
}

Pattern parsePattern(const Pattern word, const char* const text) {
	assert(basicValidate(text));

	Pattern out = ANYTHING;

	// have to check before, to remove from the entire pattern without affecting duplicates
	// Duplicates that are in the guess but not the word will have the first instance be yellow,
	// and any subsequent instances be gray. Don't remove those, so count backwards.
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
		if (text[i] == 'g') {
			out.data[i] = word.data[i]; // must be exact
			incrLowerBound(&out, word.data[i]);

		} else if (text[i] == 'y') {
			// anything but the letter
			out.data[i] &= ~word.data[i];
			incrLowerBound(&out, word.data[i]);

		}
	}

	// do after so that all lower bound are set correctly
	for (uint i = 0; i < 5; i++) {
		if (text[i] == 'n')
			setBoundsEqual(&out, word.data[i]);
	}

	return out;
}

Pattern readPattern(const Pattern word, const Config* const config) {
	// if we're using a predetermined solution to test the algorithm
	if (config->solution)
		return simGuess(word, str2pattern(config->solution));

	bool inputIsValid = false;
	Pattern out;

	while (!inputIsValid) {
		char wordStr[6];
		wordStr[5] = 0;
		pattern2str(word, wordStr);
		printf("Guessed: %s\n", wordStr);

		char* input;
		scanf("%ms", &input);
		inputIsValid = validateInput(input);

		if (inputIsValid) {
			out = parsePattern(word, input);
		} else {
			printf("Invalid input. Please enter a 5 character string where:\n"
					"\t'g' means the letter was an exact match (green)\n"
					"\t'y' means the letter was a partial match (yellow)\n"
					"\t'n' means the letter was not in the solution at all (gray).\n");
		}

		free(input);
	}
	return out;
}
