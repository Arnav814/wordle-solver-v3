#include <stdio.h>
#include "wordlist.h"
#include "algorithm.h"

int main() {
	Wordlist words = loadWordlist();
	Pattern p = {char2letter('a') | char2letter('b'), ANY_LETTER, ANY_LETTER, ANY_LETTER, ANY_LETTER};
	Wordlist newWords = filter(p, words);
	printWords(words);

	printf("Starting with a or b (%i total): ", countFilter(p, words)); // ineffient, but a good test
	printWords(newWords);

	free(words.data);
	free(newWords.data);
}

