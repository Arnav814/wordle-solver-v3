#ifndef USERIO_H
#define USERIO_H
#include "pattern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// turns the string in text into a pattern, also takes the word the user is responding to
// text may or may not be null terminated, but needs at least 5 chars
// n for no match, y for yellow, and g for green
// TODO: handle duplicated yellows properly
// TODO: proper error handling
Pattern parsePattern(const Pattern word, const char* const text) {
	// bad, placeholder error handling
	for (uint i = 0; i < 5; i++) {
		assert(text[i] == 'g' || text[i] == 'y' || text[i] == 'n');
	}

	Pattern out = {ANY_LETTER, ANY_LETTER, ANY_LETTER, ANY_LETTER, ANY_LETTER};
	for (uint i = 0; i < 5; i++) {
		if (text[i] == 'g')
			out.data[i] = word.data[i]; // must be exact
		else if (text[i] == 'y')
			out.data[i] = ~word.data[i]; // anything but the letter, TODO: handle better
	}

	// have to check after, to remove from the entire pattern
	for (uint i = 0; i < 5; i++) {
		if (text[i] == 'n') {
			for (uint k = 0; k < 5; k++) {
				out.data[k] &= ~word.data[i]; // remove word[i] from the entire pattern
			}
		}
	}

	return out;
}

// reads a pattern from stdin, prompting the user with word
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

#endif /* USERIO_H */
