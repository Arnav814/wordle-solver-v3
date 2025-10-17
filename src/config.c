#include "config.h"
#include "fsutils.h"
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <argparse.h>
#include <string.h>

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
			relPath = recursivelySearch(config->searchPath[i], path);
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
	// APPEND(strdup("/usr/l"));

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

Config* configParse(int argc, char** argv) {
	Config* config = calloc(1, sizeof(Config));
	config->wordsFile = "../wordlists/long.txt";
	config->solutionsFile = NULL; // set default later, in case wordsFile is changed
	config->jobs = 1;
	config->verbosity = 1;
	config->solution = NULL;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_STRING('w', "words", &config->wordsFile, "File for all guessable words.", NULL, 0, 0),
		OPT_STRING('s', "solutions", &config->solutionsFile, "File for all possible solutions.", NULL, 0, 0),
		OPT_INTEGER('j', "jobs", &config->jobs, "Number of threads to use.", NULL, 0, 0),
		OPT_INTEGER('v', "verbosity", &config->verbosity, "How verbose to be from 0 to 3.", NULL, 0, 0),
		OPT_STRING('a', "autoscore", &config->solution, "Assume this word is the solution and run without input.", NULL, 0, 0),
		OPT_END(),
	};
	const char* const usages[] = {
		"prog [options]",
		NULL,
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argc = argparse_parse(&argparse, argc, (const char**) argv);

	parseConfigPath(config);

	config->wordsFile = lookupWordlist(config->wordsFile, config);

	// if not set, default to the wordlist (or if they're the same)
	if (config->solutionsFile == NULL || strcmp(config->solutionsFile, config->wordsFile) == 0)
		config->solutionsFile = config->wordsFile;
	else
		config->solutionsFile = lookupWordlist(config->solutionsFile, config);

	if (config->solution)
		config->solution = strdup(config->solution);

	return config;
}

void configFree(Config* config) {
	// if they're the same pointer, only free one
	if (config->wordsFile != config->solutionsFile)
		free(config->solutionsFile);
	free(config->wordsFile);

	if (config->solution)
		free(config->solution);

	for (uint i = 0; i < config->searchEntries; i++) {
		free(config->searchPath[i]);
	}
	free(config->searchPath);

	free(config);
}

