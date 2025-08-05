#include "wordlist.h"
#include "algorithm.h"
#include "userio.h"

int main() {
	Wordlist words = loadWordlist();

	Pattern guess = findBestWord(words, words);
	char wordStr[6];
	wordStr[5] = 0;
	pattern2str(guess, wordStr);
	printf("Best guess: %s.\n", wordStr);

	free(words.data);
}

