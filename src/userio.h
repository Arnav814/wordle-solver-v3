#ifndef USERIO_H
#define USERIO_H
#include "config.h"
#include "pattern.h"

// returns true or false depending on whether the input is valid
bool validateInput(const char* const text);

// turns the string in text into a pattern, also takes the word the user is responding to
// text may or may not be null terminated, but needs at least 5 chars
// n for no match, y for yellow, and g for green
// TODO: handle duplicated yellows properly
Pattern parsePattern(const Pattern word, const char* const text);

// depending on config->solution, either
// reads a pattern from stdin, prompting the user with word
// or genereates the pattern assuming config->solution is the solution
Pattern readPattern(const Pattern word, const Config* const config);

#endif /* USERIO_H */
