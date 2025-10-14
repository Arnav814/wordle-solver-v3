#ifndef PATTERN_H
#define PATTERN_H
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

// TODO: properly support repeated orange characters

// can be easily passed
typedef struct {
	// TODO: pack this
	uint data[5];
	uint counts[4];
} Pattern;

// filters anything
#define ANY_LETTER 0b11111111111111111111111111
#define ANYTHING {{ANY_LETTER, ANY_LETTER, ANY_LETTER, ANY_LETTER, ANY_LETTER}, {0, 0, 0, 0}}
#define NULL_PATTERN {0}

// size of a letter in bits
#define LETTER_SIZE 26

// Stores characters as an int, where all letters have a corresponding bit.
// This way, words and patterns have the same representation, composing
// patterns is a bitwise and, and checking a patters is bitwise and then checking
// if any digits are zero. If so, the pattern doesn't match.

// only works on lowercase chars
int char2letter(const char c);

// will fail if called on a pattern with multiple choices for one letter
char letter2char(const uint letter);

// provided string may or may not be null terminated, but must be >= 5 chars
Pattern str2pattern(const char* word);

// sets the provided res variable to the pattern's word content, not null-terminated
void pattern2str(const Pattern pattern, char* res);

// for debugging
void printPattern(const Pattern pattern);

// returns the output
Pattern composePatterns(const Pattern a, const Pattern b);

void incrLowerBound(Pattern* const pattern, const uint letter);

// set the upper bound equal to the lower bound
void setBoundsEqual(Pattern* const pattern, const uint letter);

// check that word matches pattern
// inlined to avoid function call overhead
static inline bool checkPattern(const Pattern pattern, const Pattern word) {
	for (uint i = 0; i < 5; i++) {
		if ((pattern.data[i] & word.data[i]) == 0)
			return false;
	}
	for (uint i = 0; i < 4; i++) {
		if ((pattern.counts[i] & word.counts[i]) == 0)
			return false;
	}
	return true;
}

#endif /* PATTERN_H */
