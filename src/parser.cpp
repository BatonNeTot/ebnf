#include "parser.h"

namespace ebnf {

	Parser::Parser(const Ebnf& ebnf, Ebnf::IdInfo& info)
		: _ebnf(ebnf), _info(info) {}

	std::pair<bool, Token> Parser::parseIncremental(const std::string& str) const {
		StateStack stack;

		Cache cache;
		SourceInfo mostSuccessfulSource(str);
		StateInfo* failedNode = nullptr;

		stack.push(_info.tree, str);

		while (!stack.empty() && stack.top().node != nullptr) {
			auto& state = stack.top();

			if (!parseIncrementalStep(state, stack, cache, mostSuccessfulSource, failedNode)) {
				failedNode = &state;
				stack.pop();
			}
		}

		if (!stack.empty()) {
			return std::make_pair<bool, Token>(true, stack.buildTokens(_ebnf));
		}
		else {
			return std::make_pair<bool, Token>(false, Token(mostSuccessfulSource.line(), mostSuccessfulSource.offset()));
		}
	}

	bool Parser::parseIncrementalStep(StateInfo& state, StateStack& stack, Cache& cache, SourceInfo& mostSuccessfulSource, StateInfo*& failedNode) const {
		auto newState = state.node->incrementState(state, failedNode);
		if (newState == 0) {
			recordFailerCache(state, cache);
			return false;
		}

		failedNode = nullptr;
		state.value = newState;

		if (state.node->isCacheable()) {
			auto* record = cache.checkRecord(state.node, state.source);
			if (record != nullptr) {
				if (!record->success) {
					return false;
				}

				StateInfo* nextParentState = nullptr;
				auto* next = state.node->siblingOrNext(_ebnf, state, nextParentState);
				return pushNext(next, nextParentState, state, stack, cache, record->source);
			}
		}

		SourceInfo updatedSource = state.source;
		auto success = state.node->updateStr(_ebnf, updatedSource);
		if (!success) {
			recordFailerCache(state, cache);
			return false;
		}

		if (updatedSource.str().size() < mostSuccessfulSource.str().size()) {
			mostSuccessfulSource = updatedSource;
		}

		StateInfo* nextParentState = nullptr;
		auto* next = state.node->next(_ebnf, state, nextParentState);
		return pushNext(next, nextParentState, state, stack, cache, std::move(updatedSource));
	}

	void Parser::recordSuccessCache(StateInfo& state, StateInfo& nextParentState, Cache& cache, const SourceInfo& source) const {
		StateInfo* itState = &state;
		while (itState != &nextParentState) {
			if (itState->node->isCacheable()) {
				cache.recordSuccess(itState->node, itState->source, source);
			}

			itState = itState->parent;
		}
	}

	void Parser::recordFailerCache(StateInfo& state, Cache& cache) const {
		if (state.node->isCacheable()) {
			cache.recordFailer(state.node, state.source);
		}
	}

	bool Parser::pushNext(Node* next, StateInfo* nextParentState, StateInfo& state, StateStack& stack, Cache& cache, SourceInfo nextSource) const {
		if (next != nullptr) {
			recordSuccessCache(state, *nextParentState, cache, nextSource);

			auto childIndex = nextParentState == state.parent ? state.childIndex + 1 : 0;
			stack.push(next, nextParentState, childIndex, std::move(nextSource));
			return true;
		}

		if (nextSource.str().empty()) {
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

		std::string output;

		stack.push(_info.tree);

		while (true) {
			auto& state = stack.top();
			auto newState = state.node->incrementState(state, nullptr);
			do {
				state.value = newState;
				newState = state.node->incrementState(state, nullptr);
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