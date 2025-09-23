#ifndef CACHE_H
#define CACHE_H
#include "algorithm.h"
#include "config.h"

// avoid having things that include this also include jansson
typedef struct Cache Cache;

// load the cache from disk
Cache* cacheInit();

// takes in the paths to the cache and the wordlist in the config object
// if the entry isn't found, then returns all 0s for the word and 0 for score
BestWord cacheGet(const Cache* const cache, const Config* const config);

// add the word to the cache, using config as context for when to use this entry
// immediately written to disk
void cacheSet(Cache* const cache, const Config* const config, const BestWord word);

void cacheFree(Cache* cache);

#endif /* CACHE_H */
