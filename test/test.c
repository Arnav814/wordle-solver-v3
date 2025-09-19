#include "../src/wordlist.h"
#include "../src/algorithm.h"
#include "../src/cliparse.h"
#include "../src/userio.h"
#include <stdlib.h>
#include <unity.h>

Config* config;

Wordlist shortList;
Wordlist mediumList;
Wordlist longList;

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

	shortList = loadWordlist("../wordlists/short.txt");
	mediumList = loadWordlist("../wordlists/medium.txt");
	longList = loadWordlist("../wordlists/wordlist.txt");
}

void tearDown(void) {
	configFree(config);
	free(shortList.data);
	free(mediumList.data);
	free(longList.data);
}

void test_load_wordlist() {
	TEST_ASSERT_TRUE(wordlistHasValue(shortList));
	TEST_ASSERT_TRUE(wordlistHasValue(mediumList));
	TEST_ASSERT_TRUE(wordlistHasValue(longList));
}

void test_wordlist_copy() {
	Wordlist copied = copyWordlist(mediumList);
	TEST_ASSERT_TRUE(wordlistsAreEqual(mediumList, copied));

	free(copied.data);
}

void test_filter_repeat() {
	Pattern p = parsePattern(str2pattern("apple"), "gnnyn");

	// running something through the same filter twice should be the same
	Wordlist filtered1 = filter(p, longList);
	Wordlist filtered2 = filter(p, filtered1);

	TEST_ASSERT_TRUE(wordlistHasValue(filtered1));
	TEST_ASSERT_TRUE(wordlistsAreEqual(filtered1, filtered2));

	free(filtered1.data);
	free(filtered2.data);
}

void test_count_equals_filter() {
	Pattern p = parsePattern(str2pattern("parts"), "nnnyn");
	Wordlist filtered = filter(p, mediumList);
	uint filteredCount = countFilter(p, mediumList);

	TEST_ASSERT_EQUAL_UINT(filtered.count, filteredCount);

	free(filtered.data);
}

void test_commutative_compose() {
	Pattern a = parsePattern(str2pattern("parts"), "nnnyn");
	Pattern b = parsePattern(str2pattern("apple"), "nnngy");
	Wordlist filteredA = filter(a, longList);
	Wordlist filteredB = filter(b, filteredA);

	Pattern composed = composePatterns(a, b);
	Wordlist filteredComposed = filter(composed, longList);

	TEST_ASSERT_TRUE(wordlistHasValue(filteredComposed));
	TEST_ASSERT_TRUE(wordlistsAreEqual(filteredB, filteredComposed));

	free(filteredA.data);
	free(filteredB.data);
	free(filteredComposed.data);
}

void test_user_input_validation() {
	TEST_ASSERT_FALSE(validateInput("njnnn"));
	TEST_ASSERT_FALSE(validateInput("gng\0")); // too short strings will only be caught if they're null-terminated
	TEST_ASSERT_FALSE(validateInput("nnnnnn"));
	TEST_ASSERT_TRUE(validateInput("ynngn"));
	TEST_ASSERT_TRUE(validateInput("ggggg"));
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_load_wordlist);
	RUN_TEST(test_wordlist_copy);
	RUN_TEST(test_filter_repeat);
	RUN_TEST(test_count_equals_filter);
	RUN_TEST(test_commutative_compose);
	RUN_TEST(test_user_input_validation);

	return UNITY_END();
}

