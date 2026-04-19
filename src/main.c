#include "config.h"
#include "solver.h"
#include <sys/stat.h>

Config* config;

int main(int argc, char** argv) {
	config = configParse(argc, argv);

	solver(config);

	configFree(config);
}

