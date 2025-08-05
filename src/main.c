#include "wordlist.h"
#include "algorithm.h"
#include "userio.h"

int main() {
	Wordlist words = loadWordlist();

	printWords(words);

	ulong cwl = cumulativeWordsLeft(str2pattern("about"), words);
	printf("cumulativeWordsLeft: %lu\n", cwl);

	free(words.data);
}

