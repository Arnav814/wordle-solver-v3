#include "config.h"
#include "argparse.h"
#include "fsutils.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// always returns a new string
char* lookupWordlist(const char* const path, const Config* const config) {
	char* relPath = NULL; // relative path (may contain ., .., or symlinks)
	
	// if the path starts with ./ or ../ it should always be treated as a plain path
	// this is safe even with 1-char paths, because if the null terminator is found,
	// the &&s will short-circuit
	if (path[0] == '.' && (path[1] == '/' || (path[1] == '.' && path[2] == '/'))) {
		relPath = strdup(path);
	} else {
		// iterate backwards so entries specified last are searched first
		for (int i = config->searchEntries - 1; i >= 0; i--) {
			relPath = recursivelySearch(config->searchPath[i], path, config);
			if (relPath) break;
		}
	}

	if (!relPath) {
		printf("Failed to find wordlist \"%s\".\n", path);
		exit(1);
	} else {
		char* absPath = realpath(relPath, NULL);
		if (!absPath) {
			printf("Failed to resolve path \"%s\" (error %i).\n", relPath, errno);
			exit(1);
		}
		free(relPath);

		if (config->verbosity >= 4)
			printf("Using \"%s\" for specified wordlist \"%s\".\n", absPath, path);
		return absPath;
	}
}

// add the search path to the provided config struct
// this should be searched in reverse order
void parseConfigPath(Config* config) {
	const char* const userProvided = getenv("WORDLIST_PATH");

	uint capacity = 1; // possible space in array of char*
	config->searchEntries = 0; // actual number of entries
	config->searchPath = calloc(capacity, sizeof(char*));

	// reallocates if neccesary, then appends entry to config->searchPath
#	define APPEND(entry) do {\
			if (capacity <= config->searchEntries) { \
				capacity *= 2; \
				config->searchPath = reallocarray(config->searchPath, capacity, sizeof(char*)); \
			} \
\
			config->searchPath[config->searchEntries++] = entry; } while (false)

	// default paths to check
	APPEND(strdup("/usr/share/wordlebot3"));
	APPEND(strdup("/usr/local/share/wordlebot3"));

	// the user has provided a custom path
	if (userProvided && *userProvided) {
		char* wlPath = strdup(userProvided);
		char* toFree = wlPath; // keep a reference so it can be freed
		char* token;
		
		while ((token = strsep(&wlPath, ":"))) {
			if (token[0] != '/') {
				printf("Paths provided in WORDLIST_PATH must be absolute and not empty, \"%s\" is not.\n", token);
				exit(1);
			}

			APPEND(strdup(token));
		}

		free(toFree);
	}

	config->searchPath = reallocarray(config->searchPath, config->searchEntries, sizeof(char*));
#	undef APPEND
}

Config* debugConfig() {
	Config* config = calloc(1, sizeof(Config));
	config->jobs = 1;
	config->verbosity = 1;
	return config;
}

Config* configParse(int argc, char** argv) {
	Config* config = calloc(1, sizeof(Config));

	ArgDef options[] = {
		{Flag, 'h', "help", "Show this help message."},
		{Path, 'w', "words", "File for all guessable words."},
		{Path, 's', "solutions", "File for all possible solutions."},
		{ULong, 'j', "jobs", "Number of threads to use."},
		{Count, 'v', "verbosity", "How verbose to be from 0 to 3."},
		{String, 'a', "autoscore", "Assume this word is the solution and run without input."},
	};

	ArgsParsed* args = argsParse(sizeof(options) / sizeof(ArgDef), options, argc, argv);
	if (getPositionalCount(args) != 0) {
		printf("Unexpected positional argument \"%s\"\n", getPositional(args, 0));
		exit(1);
	}

	config->wordsFile = getStr(args, 'w', "long.txt");
	config->solutionsFile = getStr(args, 's', NULL); // set default later, in case wordsFile is changed
	config->jobs = getULong(args, 'j', 1);
	config->verbosity = getCount(args, 'v') + 1;
	config->solution = getStr(args, 's', NULL);

	parseConfigPath(config);

	config->wordsFile = lookupWordlist(config->wordsFile, config);

	// if not set, default to the wordlist (or if they're the same)
	if (config->solutionsFile == NULL || strcmp(config->solutionsFile, config->wordsFile) == 0)
		config->solutionsFile = config->wordsFile;
	else
		config->solutionsFile = lookupWordlist(config->solutionsFile, config);

	if (config->solution)
		config->solution = strdup(config->solution);

	argsFree(args);
	return config;
}

void configFree(Config* config) {
	// if they're the same pointer, only free one
	if (config->wordsFile != config->solutionsFile)
		free((void*)config->solutionsFile);
	free((void*)config->wordsFile);

	if (config->solution)
		free((void*)config->solution);

	for (uint i = 0; i < config->searchEntries; i++) {
		free(config->searchPath[i]);
	}
	free(config->searchPath);

	free(config);
}

