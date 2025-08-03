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

Wordlist loadWordlist() {
	FILE* file = fopen("../wordlists/wordlist.txt", "r");
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
			printf("\nreallocated\n");
			bufElems *= 2; // growth factor of 2
			lineBuf = realloc(lineBuf, bufElems * 5);
		}

		printf(".");
		fflush(stdout);
		memcpy(line, lineBuf + lineCount * 5, 5);
	}

	// convert to the correct format
	Pattern* wordData = calloc(lineCount, sizeof(Pattern));
	Wordlist words = {lineCount, wordData};

	for (uint wordIdx = 0; wordIdx < lineCount; wordIdx++) {
		words.data[wordIdx] = str2pattern(lineBuf + wordIdx * 5);
	}

	free(lineBuf);
	printf("Read %i lines, buffer size %i.\n", lineCount, bufElems);
	return words;
}

#endif /* WORDLIST_H */
