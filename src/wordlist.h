#ifndef WORDLIST_H
#define WORDLIST_H
#include "pattern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	uint count;
	Pattern* data;
} Wordlist;

// allocates memory automatically
Wordlist loadWordlist() {
	FILE* file = fopen("../wordlists/short.txt", "r");
	char line[7]; // include the newline char

	uint initialLineCount = 64; // how many words to initially allocate for
	
	// how I miss .push_back()
	char* lineBuf = calloc(initialLineCount, 5); // each line is 5 chars
	uint bufElems = initialLineCount; // size of allocated buffer
	uint lineCount = 0; // elements stored in buffer, tell when to reallocate

	while(fgets(line, sizeof(line), file)) {
		assert(line[5] == '\n'); // if it's missing, the line was truncated
		lineCount++;

		if (lineCount >= bufElems) { // reallocate if necessary
			bufElems *= 2; // growth factor of 2
			lineBuf = reallocarray(lineBuf, bufElems, 5);
		}

		memcpy(lineBuf + (lineCount - 1) * 5, &line, 5);
	}

	// convert to the correct format
	Pattern* wordData = calloc(lineCount, sizeof(Pattern));
	Wordlist words = {lineCount, wordData};

	for (uint wordIdx = 0; wordIdx < lineCount; wordIdx++) {
		words.data[wordIdx] = str2pattern(lineBuf + wordIdx * 5);
	}

	free(lineBuf);
	// printf("Read %i lines, buffer size %i.\n", lineCount, bufElems);
	return words;
}

void printWords(const Wordlist words) {
	char word[6];
	word[5] = 0; // add null terminator
	for (uint i = 0; i < words.count; i++) {
		pattern2str(words.data[i], word);
		printf("%s,", word);
	}
	printf("\n");
}

// returns a copy of the wordlist, allocating memory by itself
Wordlist copyWordlist(const Wordlist initial) {
	Pattern* copied = calloc(initial.count, sizeof(Pattern));
	memcpy(copied, initial.data, initial.count * sizeof(Pattern));
	Wordlist out = {initial.count, copied};
	return out;
}

#endif /* WORDLIST_H */
