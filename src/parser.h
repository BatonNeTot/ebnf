#ifndef parser_h
#define parser_h

#include "ebnf/ebnf.h"

namespace ebnf {

	class Parser{
	public:

		Parser(const Ebnf& ebnf, Ebnf::IdInfo& info);

		std::pair<bool, Token> parseIncremental(const std::string& str) const;

		std::string generate(float incrementChance) const;

	private:

		bool parseIncrementalStep(StateInfo& state, StateStack& stack, 
			FailerCache& cache, std::string_view& mostSuccessfulSource) const;

		bool tryRecordFailerCache(StateInfo& state, FailerCache& cache) const;

		bool pushNext(Node* next, StateInfo* nextParentState, StateStack& stack, 
			FailerCache& cache, std::string_view nextSource) const;

		const Ebnf& _ebnf;
		Ebnf::IdInfo& _info;
	};
}

#endif // !parser_h
