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
		"non-zero-digit='1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'."
		"zero='0'."
		"digit=zero|non-zero-digit."
		"integer$=non-zero-digit{digit}."
		"float-number$=[non-zero-digit{digit}|zero]'.'{digit}."
		"number=integer|float-number."

		"space=' '."
		"expression={space}equality-expression{space}."
		"equality-expression=additive-expression{{space}('=='|'!='){space}additive-expression}."
		"additive-expression=multiplicative-expression{{space}('+'|'-'){space}multiplicative-expression}."
		"multiplicative-expression=primary{{space}('*'|'/'){space}primary}."
		"primary='('expression')'|number|'-'primary."
	);

	auto token = form.parseAs("  65 ==     7 - 420.69 * 35 + 6", "expression");
	token.second.compress(form);
	std::cout << token.second.toStr() << std::endl;

	getc(stdin);
	return 0;
}