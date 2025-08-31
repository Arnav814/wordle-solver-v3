#include "wordlist.h"
#include "algorithm.h"
#include "userio.h"
#include "cliparse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config* config;

int main(int argc, char** argv) {
	config = configParse(argc, argv);

	Wordlist words = loadWordlist(config->wordsFile);

	Wordlist solutions;
	// don't reload the file from disk if it's the same
	if (strcmp(config->wordsFile, config->solutionsFile) == 0) solutions = copyWordlist(words);
	else solutions = loadWordlist(config->solutionsFile);
	Pattern knownInfo = ANYTHING;

	while (solutions.count > 1) {
		// printf("All solutions: ");
		// printWords(solutions);
		// printf("\n");

		Pattern guess = threadedFindWord(words, solutions, config->jobs).word;
		Pattern update = readPattern(guess);
		// printf("upd:");
		// printPattern(update);
		knownInfo = composePatterns(knownInfo, update);
		// printf("new:");
		// printPattern(knownInfo);

		Wordlist newSolutions = filter(knownInfo, solutions);
		free(solutions.data);
		solutions = newSolutions;
	}

	if (solutions.count == 1) {
		char solutionStr[6];
		solutionStr[5] = 0;
		pattern2str(solutions.data[0], solutionStr);
		printf("The solution is %s.\n", solutionStr);
	} else {
		printf("No more possible solutions. Either the actual solution is not in the provided "
				"wordlist, or you entered something incorrectly.\n");
	}

	free(words.data);
	free(solutions.data);
	configFree(config);
}

