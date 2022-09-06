#include "parser.h"

namespace ebnf {

	Parser::Parser(const Ebnf& ebnf, Ebnf::IdInfo& info)
		: _ebnf(ebnf), _info(info) {}

	std::pair<bool, Token> Parser::parseIncremental(const std::string& str) const {
		StateStack stack;

		FailerCache cache;
		std::string_view mostSuccessfulSource(str);

		stack.push(_info.tree, str);

		while (!stack.empty() && stack.top().node != nullptr) {
			auto& state = stack.top();

			if (!parseIncrementalStep(state, stack, cache, mostSuccessfulSource)) {
				stack.pop();
			}
		}

		if (!stack.empty()) {
			return std::make_pair<bool, Token>(true, stack.buildTokens(_ebnf));
		}
		else {
			auto parsedSource = str.substr(0, str.length() - mostSuccessfulSource.length());
			auto line = 0u;
			auto offset = parsedSource.length();

			{
				static const std::string lineSeparator = "\n";
				std::string::size_type nextLinePos = 0;

				while ((nextLinePos = parsedSource.find(lineSeparator, nextLinePos)) != std::string::npos) {
					++line;
					nextLinePos += lineSeparator.length();
					offset = parsedSource.length() - nextLinePos;
				}
			}

			return std::make_pair<bool, Token>(false, Token(line, offset));
		}
	}

	bool Parser::parseIncrementalStep(StateInfo& state, StateStack& stack, FailerCache& cache, std::string_view& mostSuccessfulSource) const {
		state.value = state.node->incrementState(state, cache);
		if (state.value == 0) {
			if (tryRecordFailerCache(state, cache) && state.parent) {
				tryRecordFailerCache(*state.parent, cache);
			}
			if (state.parent) {
				--state.parent->nextChildIndex;
			}
			return false;
		}

		state.nextChildIndex = 0;

		if (cache.checkRecord(state.node, state.value, state.source)) {
			return true;
		}

		std::string_view updatedSource = state.source;
		auto success = state.node->updateStr(updatedSource);
		if (!success) {
			tryRecordFailerCache(state, cache);
			return true;
		}

		if (updatedSource.length() < mostSuccessfulSource.length()) {
			mostSuccessfulSource = updatedSource;
		}

		StateInfo* nextParentState = nullptr;
		auto* next = state.node->next(state, nextParentState);
		return pushNext(next, nextParentState, state, stack, cache, std::move(updatedSource));
	}

	bool Parser::tryRecordFailerCache(StateInfo& state, FailerCache& cache) const {
		if (state.node->readyForFailerCache(state, cache)) {
			//cache.record(state.node, state.value, state.source);
			//return true;
		}
		return false;
	}

	bool Parser::pushNext(Node* next, StateInfo* nextParentState, StateInfo& state, StateStack& stack, FailerCache& cache, std::string_view nextSource) const {
		if (next != nullptr) {
			++nextParentState->nextChildIndex;
			if (nextParentState->nextChildIndex > nextParentState->mostSuccessfulChild) {
				nextParentState->mostSuccessfulChild = nextParentState->nextChildIndex;
			}
			stack.push(next, nextParentState, std::move(nextSource));
			return true;
		}

		if (nextSource.empty()) {
			stack.push(nullptr);
			return true;
		}

		return false;
	}

	std::pair<bool, Token> Parser::parseGreedy(const std::string& str) const {
		return parseIncremental(str);
	}

	static float randDouble() {
		auto rand = static_cast<float>(::rand());
		return rand / RAND_MAX;
	}

	std::string Parser::generate(float incrementChance) const {
		StateStack stack;

		FailerCache cache;
		std::string output;

		stack.push(_info.tree);

		while (true) {
			auto& state = stack.top();
			auto newState = state.node->incrementState(state, cache);
			do {
				state.value = newState;
				newState = state.node->incrementState(state, cache);
			} while (newState != 0 && randDouble() < incrementChance);

			output += state.node->body();

			StateInfo* nextParentState = nullptr;
			auto* next = state.node->next(state, nextParentState);

			if (next == nullptr) {
				return output;
			}

			stack.push(next, nextParentState);
		}
	}

}