#include "pattern.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

int char2letter(const char c) {
	assert(('a' <= c) && (c <= 'z'));
	return 1 << (c - 'a');
}

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

	return ' '; // unreachable, but silences warning
}

// sets the count bytes on the provided pattern where word is the same as pattern in word format
// only operates on actual words (only one bit set per element)
void refreshCounts(Pattern* const pattern, const char* word) {
	pattern->counts[0] = ANY_LETTER; // start with 0 of all letters
	for (uint i = 1; i < 4; i++) {
		pattern->counts[i] = 0;
	}

	for (uint i = 0; i < 5; i++) {
		// count indicates the number of occurences
		// count backward, then see what's first set
		for (int count = 4; count >= 0; count--) {
			// if we've gotten all the way to the start
			if (count == 0) {
				pattern->counts[count] |= char2letter(word[i]); // set the bit
			// if the bit indicating this count is set
			} else if ((pattern->counts[count - 1] & char2letter(word[i]))) {
				pattern->counts[count - 1] &= ~char2letter(word[i]); // unset this bit
				if (count != 4) // if we're at the very end, there isn't anything to set
					pattern->counts[count] |= char2letter(word[i]); // set the next one
				break;
			}
		}
	}
}

Pattern str2pattern(const char* word) {
	Pattern pattern;
	// set the simple letters part of the pattern
	for (uint i = 0; i < 5; i++) {
		pattern.data[i] = char2letter(word[i]);
	}

	refreshCounts(&pattern, word);

	return pattern;
}

void pattern2str(const Pattern pattern, char* res) {
	for (uint i = 0; i < 5; i++) {
		res[i] = letter2char(pattern.data[i]);
	}
}

// print a single uint from a pattern
void printChar(const uint letter) {
	for (int bitIdx = sizeof(uint) * CHAR_BIT - 1; bitIdx >= 0; bitIdx--) {
		if ((letter >> bitIdx) & 1)
			printf("%c", bitIdx + 'a');
		else
			printf("_");
	}
}

void printPattern(const Pattern pattern) {
	for (uint letterIdx = 0; letterIdx < 5; letterIdx++) {
		printChar(pattern.data[letterIdx]);
		printf(",");
	}

	// enclose the count in parens
	printf("(");
	for (uint countIdx = 0; countIdx < 4; countIdx++) {
		printChar(pattern.counts[countIdx]);
		printf(",");
	}
	printf(")");

	printf("\n");
}

Pattern composePatterns(const Pattern a, const Pattern b) {
	Pattern out;
	for (uint i = 0; i < 5; i++) {
		out.data[i] = a.data[i] & b.data[i];
	}
	for (uint i = 0; i < 4; i++) {
		out.counts[i] = a.counts[i] | b.counts[i];
	}

	return out;
}

void incrLowerBound(Pattern* const pattern, const uint letter) {
	for (uint i = 0; i < 4; i++) {
		// find the first unset (allowed) bit
		if (!(pattern->counts[i] & letter)) {
			pattern->counts[i] |= letter; // set (disallow) the bit
			return;
		}
	}
}

// set the upper bound equal to the lower bound
void setBoundsEqual(Pattern* const pattern, const uint letter) {
	for (int i = 3; i >= 0; i--) {
		// find last disallowed bit (1 beyond lower bound)
		if (pattern->counts[i] & letter || i == 0) {
			// if this bit is disallowed (we've hit the lower bound), backtrack and allow the 
			// previous bit (otherwise we'd completely disallow this letter)
			if (i != 3) pattern->counts[i + 1] &= ~letter;
			return;
		} else {
			// disallow everything else
			pattern->counts[i] |= letter;
		}
	}
}

