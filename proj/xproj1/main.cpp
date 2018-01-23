//#pragma once

#include "stringSet.h"

int main(void) {

	Stringset stringset;
	stringset.readFile();
	//stringset.print();
	stringset.pageRank();
	//stringset.print();
	stringset.invertedIndex();
	//int v;
	//std::cin >> v;

	return 0;
}
