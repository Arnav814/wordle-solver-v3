#ifndef PATTERN_H
#define PATTERN_H
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <limits.h>

// TODO: properly support repeated orange characters

// can be easily passed
typedef struct {
	uint data[5];
} Pattern;

// filters anything
#define ANY_LETTER UINT_MAX

// Stores characters as an int, where all letters have a corresponding bit.
// This way, words and patterns have the same representation, composing
// patterns is a bitwise and, and checking a patters is bitwise and then checking
// if any digits are zero. If so, the pattern doesn't match.

// only works on lowercase chars
int char2letter(const char c) {
	assert(('a' <= c) && (c <= 'z'));
	return 1 << (c - 'a');
}

// will fail if called on a pattern with multiple choices for one letter
char letter2char(const uint letter) {
	// there has GOT to be a better way to do this
	switch (letter) {
		case 0b00000000000000000000000001: return 'a';
		case 0b00000000000000000000000010: return 'b';
		case 0b00000000000000000000000100: return 'c';
		case 0b00000000000000000000001000: return 'd';
		case 0b00000000000000000000010000: return 'e';
		case 0b00000000000000000000100000: return 'f';
		case 0b00000000000000000001000000: return 'g';
		case 0b00000000000000000010000000: return 'h';
		case 0b00000000000000000100000000: return 'i';
		case 0b00000000000000001000000000: return 'j';
		case 0b00000000000000010000000000: return 'k';
		case 0b00000000000000100000000000: return 'l';
		case 0b00000000000001000000000000: return 'm';
		case 0b00000000000010000000000000: return 'n';
		case 0b00000000000100000000000000: return 'o';
		case 0b00000000001000000000000000: return 'p';
		case 0b00000000010000000000000000: return 'q';
		case 0b00000000100000000000000000: return 'r';
		case 0b00000001000000000000000000: return 's';
		case 0b00000010000000000000000000: return 't';
		case 0b00000100000000000000000000: return 'u';
		case 0b00001000000000000000000000: return 'v';
		case 0b00010000000000000000000000: return 'w';
		case 0b00100000000000000000000000: return 'x';
		case 0b01000000000000000000000000: return 'y';
		case 0b10000000000000000000000000: return 'z';
		default: assert(false);
	}
}

// provided string may or may not be null terminated, but must be >= 5 chars
Pattern str2pattern(const char* word) {
	Pattern pattern = {};
	for (uint i = 0; i < 5; i++) {
		pattern.data[i] = char2letter(word[i]);
	}
	return pattern;
}

// sets the provided res variable to the pattern's word content, not null-terminated
void pattern2str(const Pattern pattern, char* res) {
	for (uint i = 0; i < 5; i++) {
		res[i] = letter2char(pattern.data[i]);
	}
}

// populates provided buffer res with the output
void composePatterns(const Pattern a, const Pattern b, Pattern res) {
	for (uint i = 0; i < 5; i++) {
		res.data[i] = a.data[i] & b.data[i];
	}
}

// check that word matches pattern
bool checkPattern(const Pattern pattern, const Pattern word) {
	for (uint i = 0; i < 5; i++) {
		if ((pattern.data[i] & word.data[i]) == 0)
			return false;
	}
	return true;
}

#endif /* PATTERN_H */
