#include "wordlist.h"
#include "algorithm.h"
#include "userio.h"
#include "cliparse.h"
#include <stdio.h>
#include <stdlib.h>

Config* config;

int main(int argc, char** argv) {
	config = configParse(argc, argv);

	Wordlist words = loadWordlist();
	Wordlist solutions = copyWordlist(words);
	Pattern knownInfo = ANYTHING;

	uint iteration = 0;
	while (solutions.count > 1) {
		// printf("All solutions: ");
		// printWords(solutions);
		// printf("\n");

		Pattern guess;
		if (iteration == 0) guess = str2pattern("lares"); // first word is really slow
		else guess = threadedFindWord(words, solutions, config->jobs).word;
		Pattern update = readPattern(guess);
		// printf("upd:");
		// printPattern(update);
		knownInfo = composePatterns(knownInfo, update);
		// printf("new:");
		// printPattern(knownInfo);

		Wordlist newSolutions = filter(knownInfo, solutions);
		free(solutions.data);
		solutions = newSolutions;

		iteration++;
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

