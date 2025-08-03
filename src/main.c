#include <stdio.h>
#include "wordlist.h"

int main() {
	Wordlist words = loadWordlist();
	printWords(words);
}

