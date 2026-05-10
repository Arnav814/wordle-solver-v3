#ifndef ARGPARSE_H
#define ARGPARSE_H
#include <stdbool.h>
#include <sys/types.h>

typedef enum {
	Long, ULong, String, Path, Count, Flag
} ArgType;

typedef struct {
	ArgType type;
	char short_name;
	char* long_name;
	char* usage;
} ArgDef;

typedef struct ArgsParsed ArgsParsed;

ArgsParsed* argsParse(const unsigned int argdefCount, const ArgDef* const argdefs,
                      int argc, char** argv);
void argsFree(ArgsParsed* args);

// works for any type
uint getCount(const ArgsParsed* parse, char c);

long getLong(const ArgsParsed* parse, char c, long fallback);
ulong getULong(const ArgsParsed* parse, char c, ulong fallback);

// also works for paths
const char* getStr(const ArgsParsed* parse, char c, const char* fallback);

uint getPositionalCount(const ArgsParsed* parse);
const char* getPositional(const ArgsParsed* parse, uint index);

#endif /* ARGPARSE_H */
