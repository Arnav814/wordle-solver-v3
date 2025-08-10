#include "algorithm.h"
#include "userio.h"
#include <limits.h>
#include <progressbar/progressbar.h>
#include <assert.h>

Wordlist filter(const Pattern pattern, const Wordlist words) {
  // allocate space assuming all words pass; probably too much space, but who
  // cares
  Pattern *outBuf = calloc(words.count, sizeof(Pattern));
  uint destIdx = 0;
  for (uint srcIdx = 0; srcIdx < words.count; srcIdx++) {
    if (checkPattern(pattern, words.data[srcIdx])) {
      memcpy(&outBuf[destIdx], &words.data[srcIdx], sizeof(Pattern));
      destIdx++;
    }
  }

  // shrink buffer to correct size; no +1 because destIdx is incremented after
  // the copy
  outBuf = reallocarray(outBuf, destIdx, sizeof(Pattern));
  Wordlist out = {destIdx, outBuf};
  return out;
}

uint countFilter(const Pattern pattern, const Wordlist words) {
	uint matching = 0;
	for (uint i = 0; i < words.count; i++) {
		if (checkPattern(pattern, words.data[i])) {
			matching++;
		}
	}
	return matching;
}

Pattern simGuess(const Pattern guess, Pattern solution) {
	Pattern guessCopy = guess; // need to keep the original guess around

	// contains the [gyn] result data, see userio.h for more
	char result[5] = {'n', 'n', 'n', 'n', 'n'};

	// see https://www.reddit.com/r/wordle/comments/ry49ne/illustration_of_what_happens_when_your_guess_has/
	// for more info about the intracacies of this code when dealing with repeated letters

	// pass for greens
	for (uint i = 0; i < 5; i++) {
		if (solution.data[i] == guessCopy.data[i]) {
			result[i] = 'g';
			// don't double count
			guessCopy.data[i] = 0;
			solution.data[i] = 0;
		}
	}

	// pass for yellows
	for (uint i = 0; i < 5; i++) {
		for (uint k = 0; k < 5; k++) {
			if (guessCopy.data[i] == solution.data[k] && guessCopy.data[i] && solution.data[k]) {
				result[i] = 'y';
				// don't double count
				guessCopy.data[i] = 0;
				solution.data[k] = 0;
			}
		}
	}

	// char resStr[6];
	// resStr[5] = 0;
	// memcpy(resStr, result, 5);
	// printf("%s", resStr);

	return parsePattern(guess, result);
}

ulong cumulativeWordsLeft(const Pattern guess, const Wordlist solutions) {
	ulong total = 0;
	for (uint solutionIdx = 0; solutionIdx < solutions.count; solutionIdx++) {
		Pattern pattern = simGuess(guess, solutions.data[solutionIdx]);
		uint filteredCount = countFilter(pattern, solutions);
		total += filteredCount;
		
		// char guessStr[6];
		// guessStr[5] = 0;
		// pattern2str(solutions.data[solutionIdx], guessStr);
		// printf("### Adding %i for %s\n", filteredCount, guessStr);

		// Wordlist actualWords = filter(pattern, solutions);
		// printWords(actualWords);
		// free(actualWords.data);
	}

	return total;
}

Pattern findBestWord(const Wordlist allWords, const Wordlist solutions) {
	assert(allWords.count > 0);
	assert(solutions.count > 0);

	Pattern bestWord;
	ulong lowestScore = ULONG_MAX;
	char label[100];
	progressbar* progress = progressbar_new("", allWords.count);

	for (uint guessIdx = 0; guessIdx < allWords.count; guessIdx++) {
		sprintf(label, "Processing %i/%i", guessIdx+1, allWords.count);
		progressbar_update_label(progress, label);

		ulong score = cumulativeWordsLeft(allWords.data[guessIdx], solutions);
		if (score < lowestScore) {
			bestWord = allWords.data[guessIdx];
			lowestScore = score;
		}

		progressbar_inc(progress);
	}

	progressbar_finish(progress);
	return bestWord;
}

