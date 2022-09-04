#ifndef parser_h
#define parser_h

#include "ebnf/ebnf.h"

namespace ebnf {

	class Parser{
	public:

		Parser(const Ebnf& ebnf, Ebnf::IdInfo& info);

		std::pair<bool, Token> parseIncremental(const std::string& str) const;

		std::pair<bool, Token> parseGreedy(const std::string& str) const;

		std::string generate(float incrementChance) const;

	private:

		bool parseIncrementalStep(StateInfo& state, StateStack& stack, 
			Cache& cache, SourceInfo& mostSuccessfulSource, StateInfo*& failedNode) const;

		void recordFailerCache(StateInfo& state, Cache& cache) const;

		void recordSuccessCache(StateInfo& state, StateInfo& nextParentState, Cache& cache, const SourceInfo& source) const;

		bool pushNext(Node* next, StateInfo* nextParentState, StateInfo& state, StateStack& stack, 
			Cache& cache, SourceInfo nextSource) const;

		const Ebnf& _ebnf;
		Ebnf::IdInfo& _info;
	};
}

#endif // !parser_h
