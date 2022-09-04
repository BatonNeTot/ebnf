#ifndef parser_h
#define parser_h

#include "ebnf/ebnf.h"

namespace ebnf {

	class Parser{
	public:

		Parser(const Ebnf& ebnf, Ebnf::IdInfo& info);

		std::pair<bool, Token> parse(const std::string& str) const;

		std::string generate(float incrementChance) const;

	private:
		const Ebnf& _ebnf;
		Ebnf::IdInfo& _info;
	};
}

#endif // !parser_h
