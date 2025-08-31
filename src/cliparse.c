#include "cliparse.h"
#include <stddef.h>
#include <stdlib.h>
#include <argparse.h>
#include <string.h>

Config* configParse(int argc, char** argv) {
	Config* config = calloc(1, sizeof(Config));
	config->wordsFile = strdup("../wordlists/wordlist.txt");
	config->solutionsFile = config->wordsFile;
	config->jobs = 1;
	config->verbosity = 1;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_STRING('w', "words", &config->wordsFile, "File for all guessable words.", NULL, 0, 0),
		OPT_STRING('s', "solutions", &config->solutionsFile, "File for all possible solutions.", NULL, 0, 0),
		OPT_INTEGER('j', "jobs", &config->jobs, "Number of threads to use.", NULL, 0, 0),
		OPT_INTEGER('v', "verbosity", &config->verbosity, "How verbose to be from 0 to 3.", NULL, 0, 0),
		OPT_END(),
	};
	const char* const usages[] = {
		"prog [options]",
		NULL,
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argc = argparse_parse(&argparse, argc, (const char**) argv);

	return config;
}

void configFree(Config* config) {
	// if they're the same pointer, only free one
	if (config->wordsFile != config->solutionsFile)
		free(config->solutionsFile);
	free(config->wordsFile);
	free(config);
}

