#include "wordlist.h"
#include "algorithm.h"
#include "cliparse.h"
#include <stdlib.h>
#include <unity.h>

Config* config;

void setUp(void) {
	// configParse may modify the arguments, so keep references so they can be freed
	void* ptr1 = malloc(sizeof(char*));
	void* ptr2 = malloc(sizeof(""));

	// empty argc and argv
	int argc = 1;
	char** argv = ptr1;
	argv[0] = ptr2;
	*argv[0] = 0;

	config = configParse(argc, argv);

	free(ptr1);
	free(ptr2);
}

void tearDown(void) {
	configFree(config);
}

void test_load_wordlist() {
	Wordlist shortList = loadWordlist("../wordlists/short.txt");
	TEST_ASSERT_TRUE(wordlistHasValue(shortList));
	Wordlist mediumList = loadWordlist("../wordlists/medium.txt");
	TEST_ASSERT_TRUE(wordlistHasValue(mediumList));
	Wordlist longList = loadWordlist("../wordlists/wordlist.txt");
	TEST_ASSERT_TRUE(wordlistHasValue(longList));
}

void test_wordlist_copy() {
	Wordlist list = loadWordlist("../wordlists/medium.txt");
	Wordlist copied = copyWordlist(list);
	TEST_ASSERT_TRUE(wordlistsAreEqual(list, copied));
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_load_wordlist);
	RUN_TEST(test_wordlist_copy);

	return UNITY_END();
}

