#include "parser.h"

namespace ebnf {

	Parser::Parser(const Ebnf& ebnf, Ebnf::IdInfo& info)
		: _ebnf(ebnf), _info(info) {}

	std::pair<bool, Token> Parser::parse(const std::string& str) const {
		StateStack stack;

		SourceInfo mostSuccessfulSource(str);

		stack.push(_info.tree, str);

		while (!stack.empty()) {
			auto& state = stack.top();
			auto newState = state.node->incrementState(state.value, nullptr);
			if (newState != 0) {
				state.value = newState;
				SourceInfo lastSource = stack.top().source;
				auto success = state.node->updateStr(_ebnf, lastSource);
				if (success) {
					if (lastSource.str.size() < mostSuccessfulSource.str.size()) {
						mostSuccessfulSource = lastSource;
					}
					StateInfo* nextParentState = nullptr;
					auto* next = state.node->next(_ebnf, state, nextParentState);
					if (next != nullptr) {
						auto childIndex = nextParentState == state.parent ? state.childIndex + 1 : 0;
						stack.push(next, nextParentState, childIndex, lastSource);
						continue;
					}

					if (lastSource.str.empty()) {
						break;
					}
				}
			}

			stack.pop();
		}

		if (!stack.empty()) {
			return std::make_pair<bool, Token>(true, stack.buildTokens(_ebnf));
		}
		else {
			return std::make_pair<bool, Token>(false, Token(mostSuccessfulSource.line, mostSuccessfulSource.offset));
		}
	}

	static float randDouble() {
		auto rand = static_cast<float>(::rand());
		return rand / RAND_MAX;
	}

	std::string Parser::generate(float incrementChance) const {
		StateStack stack;

		std::string output;

		stack.push(_info.tree);

		while (true) {
			auto& state = stack.top();
			auto newState = state.node->incrementState(state.value, nullptr);
			do {
				state.value = newState;
				newState = state.node->incrementState(state.value, nullptr);
			} while (newState != 0 && randDouble() < incrementChance);

			output += state.node->body(_ebnf);

			StateInfo* nextParentState = nullptr;
			auto* next = state.node->next(_ebnf, state, nextParentState);

			if (next == nullptr) {
				return output;
			}

			auto childIndex = nextParentState == state.parent ? state.childIndex + 1 : 0;
			stack.push(next, nextParentState, childIndex);
		}
	}

}