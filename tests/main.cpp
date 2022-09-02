#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include <exception>
#include <functional>
#include <cassert>
#include <typeindex>

#include "parser.h"
#include "ebnf.h"

int main(int argc, char** argv) {

	ebnf::Ebnf form(
		"start='1'|'12'."
		"end='23'."
		"total=start end."
	);

	auto [success, token] = form.parseAs("123", "total"); //TODO
	std::cout << std::boolalpha << success << " " << token.toStr() << std::endl;

	getc(stdin);
	return 0;
}