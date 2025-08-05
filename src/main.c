#include "wordlist.h"
#include "algorithm.h"
#include "userio.h"

int main() {
	Wordlist words = loadWordlist();

	printWords(words);
	Pattern supplied = readPattern(str2pattern("about"));
	Wordlist filtered = filter(supplied, words);
	printWords(filtered);

	free(words.data);
	free(filtered.data);
}

