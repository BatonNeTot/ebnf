#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include <exception>
#include <functional>
#include <cassert>
#include <typeindex>

#include "ebnf/ebnf.h"

int main(int argc, char** argv) {

	ebnf::Ebnf form(
		"constant='1'."
		"start={constant} constant|''."
		"end='31'|'311'."
		"total=start end."
	);

	auto [success, token] = form.parseAs("311", "total", ebnf::ParsingType::Greedy);
	token.compress(form);
	std::cout << std::boolalpha << success << ": " << token.toStr() << std::endl;
	std::string randomForm = "end total start";
	std::cout << randomForm << ": " << form.generateFor(randomForm, 0.2f) << std::endl;

	getc(stdin);
	return 0;
}