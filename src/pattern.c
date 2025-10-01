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
// only operates on actual words (one bit set per uint)
void refreshCounts(Pattern* const pattern, const char* word) {
	for (uint i = 5; i < 9; i++) {
		pattern->data[i] = 0;
	}

	for (uint i = 0; i < 5; i++) {
		// count indicates the number of occurences
		// count backward, then see what's first set
		for (int count = 4; count >= 0; count--) {
			// if we've gotten all the way to the start
			if (count == 0) {
				pattern->data[5 + count] |= char2letter(word[i]); // set the bit
			// if the bit indicating this count is set
			} else if ((pattern->data[4 + count] & char2letter(word[i]))) {
				pattern->data[4 + count] &= ~char2letter(word[i]); // unset this bit
				if (count != 4) // if we're at the very end, there isn't anything to set
					pattern->data[5 + count] |= char2letter(word[i]); // set the next one
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

void printPattern(const Pattern pattern) {
	for (uint letterIdx = 0; letterIdx < 9; letterIdx++) {
		for (int bitIdx = sizeof(uint) * CHAR_BIT - 1; bitIdx >= 0; bitIdx--) {
			if ((pattern.data[letterIdx] >> bitIdx) & 1)
				printf("%c", bitIdx + 'a');
			else
				printf("_");
		}
		printf(",");

		// enclose the count in parens
		if (letterIdx == 4) printf("(");
		else if (letterIdx == 8) printf(")");
	}
	printf("\n");
}

Pattern composePatterns(const Pattern a, const Pattern b) {
	Pattern out;
	for (uint i = 0; i < 9; i++) {
		out.data[i] = a.data[i] & b.data[i];
	}

	return out;
}
