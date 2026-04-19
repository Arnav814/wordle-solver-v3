#include "solver.h"
#include "cache.h"
#include "userio.h"
#include "wordlist.h"
#include "algorithm.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void solver(const Config* config) {
	Wordlist words = loadWordlist(config->wordsFile);

	Wordlist solutions;
	// don't reload the file from disk if it's the same
	if (strcmp(config->wordsFile, config->solutionsFile) == 0) solutions = copyWordlist(words);
	else solutions = loadWordlist(config->solutionsFile);
	Pattern knownInfo = ANYTHING;

	Cache* cache = cacheInit();
	char lastGuess[6] = {0};

	uint iteration = 0; // count guess # from 0
	while (solutions.count > 1) {
		// printf("All solutions: ");
		// printWords(solutions);
		// printf("\n");

		BestWord guess;
		if (iteration == 0) {
			BestWord cached = cacheGet(cache, config);
			if (cached.score != 0) { // cache hit
				if (config->verbosity >= 2)
					printf("Found first word in cache.\n");
				guess = cached;
			} else { // cache miss
				if (config->verbosity >= 2)
					printf("First word not in cache. Recomputing.\n");
				guess = threadedFindWord(words, solutions, config->jobs);
				cacheSet(cache, config, guess); // write to cache
			}
		} else {
			guess = threadedFindWord(words, solutions, config->jobs);
		}

		if (config->verbosity >= 2)
			printf("There are %u possible solutions. Expecting next iteration "
					"to have ~%.2f possible solutions (score: %lu).\n",
					solutions.count, (double) guess.score / solutions.count, guess.score);

		lastGuess[5] = 0;
		pattern2str(guess.word, lastGuess);

		Pattern update = readPattern(guess.word, config);
		knownInfo = composePatterns(knownInfo, update);
		if (config->verbosity >= 3) {
			printf("upd:");
			printPattern(update);
			printf("new:");
			printPattern(knownInfo);
		}

		Wordlist newSolutions = filter(knownInfo, solutions);
		free(solutions.data);
		solutions = newSolutions;
		iteration++;
	}

	if (solutions.count == 1) {
		char solutionStr[6];
		solutionStr[5] = 0;
		pattern2str(solutions.data[0], solutionStr);

		// if we already guessed the correct word by chance, don't count another word
		if (strcmp(lastGuess, solutionStr) != 0) iteration++;
		printf("The solution is %s (%i guesses made).\n", solutionStr, iteration);
	} else {
		printf("No more possible solutions. Either the actual solution is not in the provided "
				"wordlist, or you entered something incorrectly.\n");
	}

	free(words.data);
	free(solutions.data);
	cacheFree(cache);
}

