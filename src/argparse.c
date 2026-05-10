#include "argparse.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	const ArgDef* def;
	unsigned int count;
	union {
		long as_int;
		ulong as_uint;
		const char* as_string;
	} data;
} ParsedFlag;

struct ArgsParsed {
	char* programName;

	const ArgDef* defs;
	uint defCount;

	const char** posArgs;
	uint posCount;
	uint posCapacity;

	ParsedFlag** flags;
	uint flagCount;
	uint flagCapacity;
};

static ParsedFlag* getOrAddFlag(ArgsParsed* parse, const ArgDef* def) {
	for (uint i = 0; i < parse->flagCount; i++) {
		if (parse->flags[i]->def == def) {
			return parse->flags[i];
		}
	}

	if (parse->flagCount == parse->flagCapacity) {
		// use a 2x growth factor
		parse->flagCapacity *= 2;
		parse->flags = realloc(parse->flags, sizeof(ParsedFlag) * parse->flagCapacity);
	}

	ParsedFlag* flag = malloc(sizeof(ParsedFlag));
	flag->def = def;
	flag->count = 0;
	flag->data.as_uint = 0;
	parse->flags[parse->flagCount] = flag;
	parse->flagCount++;
	return parse->flags[parse->flagCount - 1];
}

ArgsParsed* argsParse(const unsigned int argdefCount, const ArgDef* const argdefs,
                      int argc, char** argv) {
	ArgsParsed* parse = calloc(1, sizeof(ArgsParsed));
	parse->programName = argv[0];
	parse->defs = argdefs;
	parse->defCount = argdefCount;
	parse->posArgs = calloc(1, sizeof(char*));
	parse->posCount = 0;
	parse->posCapacity = 1;
	parse->flags= calloc(1, sizeof(ParsedFlag*));
	parse->flagCount = 0;
	parse->flagCapacity = 1;

#	define ERROR(fmt, ...) do { \
		printf("Error while parsing arguments: " fmt, __VA_ARGS__); \
		exit(1); \
	} while (false)

	ParsedFlag* lastFlag = NULL;
	for (int i = 1; i < argc; i++) {
		const char* token = argv[i];
		ulong len = strlen(token);

		if (lastFlag != NULL) {
			// if this element actualy needs to be consumed
			if (lastFlag->def->type != Count 
					&& lastFlag->def->type != Flag) {
				if (len > 1 && token[0] == '-')
					ERROR("Argument %s can be interpreted as a flag or an argument to -%c/--%s. "
					      "Please use the --flag=value syntax or provide a value for the flag.\n",
					      token, lastFlag->def->short_name, lastFlag->def->long_name);
			}
				

			char* end = NULL;
			switch (lastFlag->def->type) {
				case Long:
				case ULong:
					lastFlag->data.as_int = strtol(token, &end, 10);
					if (end != NULL)
						ERROR("Argument to -%c/--%s (%s) is not an integer.\n", 
						      lastFlag->def->short_name, lastFlag->def->long_name, token);
					break;

				case String:
				case Path:
					lastFlag->data.as_string = token;
					break;

				case Count:
				case Flag:
					break;
			}

			if (lastFlag->def->type == ULong) {
				long as_long = lastFlag->data.as_int;
				if (as_long < 0)
					ERROR("Argument to -%c/--%s (%ld) cannot be negative.\n", 
						  lastFlag->def->short_name, lastFlag->def->long_name, as_long);
				lastFlag->data.as_uint = as_long;
			}

			lastFlag = NULL;
			continue;
		}
		
		// TODO: allow = in names

		// check for short flags
		if (len == 2 && token[0] == '-') {
			bool processed_flag = false;
			for (uint flag_idx = 0; flag_idx < argdefCount; flag_idx++) {
				const ArgDef* def = &argdefs[flag_idx];
				char flagChar = def->short_name;
				if (token[1] == flagChar) {
					processed_flag = true;
					ParsedFlag* flag = getOrAddFlag(parse, def);
					lastFlag = flag;
					flag->count++;

					if (def->type != Count) {
						if (flag->count > 1)
							ERROR("Flag -%c/--%s specified multiple times.\n",
							      def->short_name, def->long_name);
					}
				}
			}

			// if this flag wasn't valid
			if (!processed_flag) {
				ERROR("Flag %s was not recognized.\n", token);
			}

			continue;
		}

		// check for long flags
		if (len > 2 && token[0] == '-' && token[1] == '-') {
			bool processed_flag = false;
			for (uint flag_idx = 0; flag_idx < argdefCount; flag_idx++) {
				const ArgDef* def = &argdefs[flag_idx];
				if (strcmp(token+2, def->long_name) == 0) {
					processed_flag = true;
					ParsedFlag* flag = getOrAddFlag(parse, def);
					lastFlag = flag;
					flag->count++;

					if (def->type != Count) {
						if (flag->count > 1)
							ERROR("Flag -%c/--%s specified multiple times.\n",
							      def->short_name, def->long_name);
					}
				}
			}

			// if this flag wasn't valid
			if (!processed_flag) {
				ERROR("Flag %s was not recognized.\n", token);
			}

			continue;
		}

		if (parse->posCount == parse->posCapacity) {
			parse->posCapacity *= 2;
			parse->posArgs = realloc(parse->posArgs, sizeof(char*) * parse->posCapacity);
		}
		parse->posArgs[parse->posCount] = token;
		parse->posCount++;
	}

	// TODO: realloc array to avoid wasting memory
	return parse;
}

static const ArgDef* getDef(const ArgsParsed* parse, char short_name) {
	for (uint i = 0; i < parse->defCount; i++) {
		if (parse->defs[i].short_name == short_name) {
			return &parse->defs[i];
		}
	}
	assert(false);
}

static ParsedFlag* getEntry(const ArgsParsed* parse, const ArgDef* def) {
	for (uint i = 0; i < parse->flagCount; i++) {
		if (parse->flags[i]->def == def) {
			return parse->flags[i];
		}
	}
	return NULL;
}

uint getCount(const ArgsParsed* parse, char c) {
	ParsedFlag* entry = getEntry(parse, getDef(parse, c));
	if (entry == NULL) return 0;
	return entry->count;
}

long getLong(const ArgsParsed* parse, char c, long fallback) {
	const ArgDef* def = getDef(parse, c);
	assert(def->type == Long);
	ParsedFlag* entry = getEntry(parse, def);
	if (entry == NULL) return fallback;
	return entry->data.as_int;
}

ulong getULong(const ArgsParsed* parse, char c, ulong fallback) {
	const ArgDef* def = getDef(parse, c);
	assert(def->type == ULong);
	ParsedFlag* entry = getEntry(parse, def);
	if (entry == NULL) return fallback;
	return entry->data.as_uint;
}

const char* getStr(const ArgsParsed* parse, char c, const char* fallback) {
	const ArgDef* def = getDef(parse, c);
	assert(def->type == String || def->type == Path);
	ParsedFlag* entry = getEntry(parse, def);
	if (entry == NULL) return fallback;
	return entry->data.as_string;
}

uint getPositionalCount(const ArgsParsed* parse) {
	return parse->posCount;
}

const char* getPositional(const ArgsParsed* parse, uint index) {
	return parse->posArgs[index];
}

void argsFree(ArgsParsed* args) {
	// TODO: finish this
	free(args);
}

