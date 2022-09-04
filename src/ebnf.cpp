#include "ebnf/ebnf.h"

#include "ebnf_parser.h"
#include "ebnf_exp_parser.h"
#include "parser.h"

namespace ebnf {

	Ebnf::Ebnf(const std::string& source) {
		EbnfParser parser(source);

		while (parser.hasNext()) {
			auto info = parser.proceedNext();
			auto& id = info.id;
			if (!id.empty()) {
				_ids.try_emplace(id, std::move(info));
			}
		}
	}

	std::string Ebnf::generateFor(const std::string& expression, float incrementChance) const {
		IdInfo info;
		EbnfExpParser expParser(info);
		expParser.fillInfo(expression);

		Parser iterator(*this, info);
		return iterator.generate(incrementChance);
	}

	std::pair<bool, Token> Ebnf::parseAs(const std::string& str, const std::string& expression, ParsingType type) const {
		IdInfo info;
		EbnfExpParser expParser(info);
		expParser.fillInfo(expression);

		Parser iterator(*this, info);
		switch (type) {
		case ParsingType::Incremental: 
			return iterator.parseIncremental(str);
		case ParsingType::Greedy:
			return iterator.parseGreedy(str);
		}
		return std::make_pair<bool, Token>(false, {});
	}

}