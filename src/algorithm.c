#include "algorithm.h"
#include "userio.h"
#include <limits.h>
#include <pthread.h>
#include <progressbar/progressbar.h>
#include <assert.h>
#include <map.h>

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
	hashmap* map = hashmap_create();
	ulong total = 0;

	for (uint solutionIdx = 0; solutionIdx < solutions.count; solutionIdx++) {
		Pattern pattern = simGuess(guess, solutions.data[solutionIdx]);

		uintptr_t cachedCount = 0;
		uint count = 0;
		if (hashmap_get(map, hashmap_static_arr(pattern.data), &cachedCount)) {
			count = cachedCount;
		} else {
			count = countFilter(pattern, solutions);
			hashmap_set(map, hashmap_static_arr(pattern.data), count);
		}

		total += count;
		
		// char guessStr[6];
		// guessStr[5] = 0;
		// pattern2str(solutions.data[solutionIdx], guessStr);
		// printf("### Adding %i for %s\n", filteredCount, guessStr);

		// Wordlist actualWords = filter(pattern, solutions);
		// printWords(actualWords);
		// free(actualWords.data);
	}

	hashmap_free(map);
	return total;
}

BestWord findBestWord(const Wordlist allWords, const Wordlist solutions) {
	assert(allWords.count > 0);
	assert(solutions.count > 0);

	Pattern bestWord;
	ulong lowestScore = ULONG_MAX;
	// char label[100];
	// progressbar* progress = progressbar_new("", allWords.count);

	for (uint guessIdx = 0; guessIdx < allWords.count; guessIdx++) {
		// sprintf(label, "Processing %i/%i", guessIdx+1, allWords.count);
		// progressbar_update_label(progress, label);

		ulong score = cumulativeWordsLeft(allWords.data[guessIdx], solutions);
		if (score < lowestScore) {
			bestWord = allWords.data[guessIdx];
			lowestScore = score;
		}

		// progressbar_inc(progress);
	}

	// progressbar_finish(progress);
	BestWord asStruct = {bestWord, lowestScore};
	return asStruct;
}

// for multithreading
typedef struct {
	Wordlist words;
	Wordlist solutions;
} WordsSolutions;

// can only use one argument for threads, has to return void*
void* findBestWordWrapper(void* arg) {
	WordsSolutions* casted = (WordsSolutions*) arg;
	BestWord bestWord = findBestWord(casted->words, casted->solutions);
	BestWord* asPtr = (BestWord*)calloc(1, sizeof(BestWord)); // have to return a void*
	memcpy(asPtr, &bestWord, sizeof(BestWord));
	return asPtr;
};

BestWord threadedFindWord(const Wordlist allWords, const Wordlist solutions, uint threadCount) {
	assert(allWords.count > 0);
	assert(solutions.count > 0);

	// don't spawn more threads than there are words
	if (allWords.count < threadCount)
		threadCount = allWords.count;

	pthread_t* threads = calloc(threadCount, sizeof(pthread_t));
	WordsSolutions* searchSpaces = calloc(threadCount, sizeof(WordsSolutions));

	// create the threads
	uint alreadyProcessed = 0; // count the words we've already made threads for
	for (uint i = 0; i < threadCount; i++) {
		// Where to process until.
		// Instead of count * (i/threads), use (count*i) / threads to avoid floats.
		uint processUntil = ((ulong) allWords.count * (i + 1)) / threadCount;
		Wordlist thisSection = {processUntil - alreadyProcessed, &allWords.data[alreadyProcessed]};

		WordsSolutions searchSpace = {thisSection, solutions};
		searchSpaces[i] = searchSpace; // keep the data after this loop iteration
		// printf("Spawning thread for %i words @ %p. From %i to %i.\n", searchSpace.words.count,
		// 		searchSpace.words.data, alreadyProcessed, processUntil);
		pthread_create(&threads[i], NULL, findBestWordWrapper, &searchSpaces[i]);

		alreadyProcessed = processUntil;
	}

	// get the results from the threads
	BestWord bestWord = {ANYTHING, ULONG_MAX};
	for (uint i = 0; i < threadCount; i++) {
		BestWord* fromThread = NULL;
		pthread_join(threads[i], (void**) &fromThread);

		// char asStr[6];
		// asStr[5] = 0;
		// pattern2str(fromThread->word, asStr);
		// printf("Thread says: %s, %lu.\n", asStr, fromThread->score);

		// keep the best word
		if (fromThread->score < bestWord.score)
			bestWord = *fromThread;
		free(fromThread);
	}

	free(threads);
	free(searchSpaces);
	return bestWord;
}

