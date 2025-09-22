#include "cache.h"
#include "fsutils.h"
#include <errno.h>
#include <jansson.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct Cache {
	char* cachePath; // path to the cache file
	json_t* cache; // actual cache in memory
};

#define CACHE_FILE_NAME "/cache.json"

char* getCachePath() {
	char* cacheDir = getXdgPath("XDG_CACHE_HOME", "/.cache");
	
	char* cachePath = malloc(strlen(cacheDir) + strlen(CACHE_FILE_NAME) + 1);
	strcpy(stpcpy(cachePath, cacheDir), CACHE_FILE_NAME); // cachePath = cacheDir + CACHE_FILE_NAME

	free(cacheDir);
	return cachePath;
}

Cache* cacheInit() {
	char* cachePath = getCachePath();
	Cache* out = calloc(1, sizeof(Cache));

	json_t* root = NULL;
	json_error_t error;
	FILE* cacheFile = fopen(cachePath, "r");
	if (!cacheFile && errno == ENOENT) { // file doesn't exist
		root = json_array();
	} else if (!cacheFile) { // some other error
		printf("Error %i when attempting to read file %s.\n", errno, cachePath);
		exit(1);
	} else { // normal operation
		if (fseek(cacheFile, 0, SEEK_END) == -1) {
			printf("Error while trying to get size of %s. (Call to fseek failed with %i.)\n",
					cachePath, errno);
			exit(1);
		}

		long size = ftell(cacheFile);
		if (size == -1) {
			printf("Error while trying to get size of %s. (Call to ftell failed with %i.)\n",
					cachePath, errno);
			exit(1);
		}
		rewind(cacheFile);

		if (size != 0) {
			root = json_loadf(cacheFile, JSON_REJECT_DUPLICATES, &error);
			if (!root) {
				printf("Error decoding JSON: %s. Encountered in %s:%i:%i (pos: %i).\n",
						error.text, error.source, error.line, error.column, error.position);
				exit(1);
			} else if (!json_is_array(root)) {
				printf("JSON cache is not formatted correctly (expected top-level array).\n");
				exit(1);
			}
		} else { // file is empty
			root = json_array();
		}

		out->cache = root;
		fclose(cacheFile);
	}

	out->cachePath = cachePath;	// don't free because it's included in the returned struct
	return out;
}

// search the cache for the entry matching the current config, returns NULL if not found
// if outIndex is not null, it will be set to the index of the found entry or not modified
// if useMTime is true, then it takes modification times into account
// returns a new reference to the JSON object
json_t* findEntry(const Cache* const cache, const Config* const config, const
		bool useMTime, size_t* outIndex) {
	size_t index;
	json_t* value;

	json_array_foreach(cache->cache, index, value) {
		json_t* wordList = json_object_get(value, "wlist");
		json_t* solutionList = json_object_get(value, "slist");

		if (!wordList || !solutionList) {
			printf("Missing attributes in cache entry #%zu.\n", index);
			exit(1);
		}

		const char* wordListPath = json_string_value(wordList);
		const char* solListPath = json_string_value(solutionList);

		if (!wordListPath || !solListPath) {
			printf("Invalid type for values in cache entry #%zu.\n", index);
			exit(1);
		}

		// check the filenames match
		if (strcmp(wordListPath, config->wordsFile) != 0 ||
				strcmp(solListPath, config->solutionsFile) != 0)
			continue;

		if (useMTime) {
			// get the modification times
			json_t* wordListMTimeJson = json_object_get(value, "wmtime");
			json_t* solListMTimeJson = json_object_get(value, "smtime");

			if (!wordListMTimeJson || ! solListMTimeJson) {
				printf("Missing modification times in cache entry #%zu.\n", index);
				exit(1);
			}

			time_t wordListMTime = json_integer_value(wordListMTimeJson);
			time_t solListMTime = json_integer_value(solListMTimeJson);

			if (wordListMTime == 0 || solListMTime == 0) {
				printf("Invalid type for modification times (or times are 0) in cache entry #%zu.\n", index);
				exit(1);
			}

			// use != instead of > because if the modification time has gone backwards, something
			// weird has happened and we should recompute things anyway
			if (getMTime(wordListPath) != wordListMTime ||
					getMTime(solListPath) != solListMTime)
				continue;
		}

		if (outIndex) *outIndex = index;
		json_incref(value);
		return value;
	}

	return NULL;
}

BestWord cacheGet(const Cache* const cache, const Config* const config) {
	json_auto_t* entry = findEntry(cache, config, true, NULL);

	// if the entry isn't in the cache
	if (!entry) {
		BestWord out = {NULL_PATTERN, 0};
		return out;
	}

	json_auto_t* word = json_object_get(entry, "word");
	json_auto_t* score = json_object_get(entry, "score");

	if (!word || !score) {
		printf("Missing attributes in cache entry.\n");
		exit(1);
	}

	const char* wordStr = json_string_value(word);
	json_int_t scoreNum = json_integer_value(score);

	if (!wordStr || scoreNum == 0) {
		// score can't be zero, because that would imply no solutions remaining
		printf("Invalid type for cache attributes in entry.\n");
		exit(1);
	}

	if (scoreNum < 0) {
		printf("Score is negative in cache entry.\n");
		exit(1);
	}

	BestWord out = {str2pattern(wordStr), scoreNum};
	return out;
}

// write the cache to disk
void writeCache(const Cache* const cache) {
	// TODO: use absolute paths so pwd doesn't affect this
	// TODO: limit cache size by removing old entries
	FILE* cacheFile = fopen(cache->cachePath, "w+");
	if (!cacheFile) {
		printf("Error %i when attempting to write to file %s.\n", errno, cache->cachePath);
		exit(1);
	}

	size_t flags;
#ifndef NDEBUG
	flags = JSON_INDENT(4); // pretty print when debugging
#else
	flags = JSON_COMPACT; // be compact for releases
#endif

	int status = json_dumpf(cache->cache, cacheFile, flags);
	if (status == -1) { // an error occured
		// I'm unsure if errno is relevant here
		printf("Error while writing JSON to %s. (maybe %i?)\n", cache->cachePath, errno);
		exit(1);
	}

	fclose(cacheFile);
}

void cacheSet(Cache* const cache, const Config* const config, const BestWord word) {
	json_auto_t* newEntry = json_object();
	json_object_set_new(newEntry, "wlist", json_string(config->wordsFile));
	json_object_set_new(newEntry, "slist", json_string(config->solutionsFile));
	json_object_set_new(newEntry, "wmtime", json_integer(getMTime(config->wordsFile)));
	json_object_set_new(newEntry, "smtime", json_integer(getMTime(config->solutionsFile)));
	json_object_set_new(newEntry, "score", json_integer(word.score));

	char wordAsStr[6];
	wordAsStr[5] = 0; // add null terminator
	pattern2str(word.word, wordAsStr);
	json_object_set_new(newEntry, "word", json_string(wordAsStr));

	size_t index;
	json_auto_t* oldEntry = findEntry(cache, config, false, &index);

	if (oldEntry) // remove the old entry, if it exists
		json_array_remove(cache->cache, index);

	json_array_append(cache->cache, newEntry);
	writeCache(cache);
}

void cacheFree(Cache* cache) {
	free(cache->cachePath);
	json_decref(cache->cache);
	free(cache);
	cache = NULL;
}

