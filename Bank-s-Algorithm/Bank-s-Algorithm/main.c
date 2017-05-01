#include "banker-s-algorithm.h"

int main(int argc, char *argv[])
{
	generate_processes();
	bankers_algo();
	destroy();
}