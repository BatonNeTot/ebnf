#include "node_brackets.h"

namespace ebnf {

	std::string NodeBrackets::toStr() const {
		return NodeSingle::toStr('(', ')');
	}

	Node* NodeBrackets::nextChild(const StateInfo& state) const {
		if (state.value == 0) {
			return nullptr;
		}

		return state.nextChildIndex == 0 ? value() : nullptr;
	}

	bool NodeBrackets::readyForFailerCache(const StateInfo& state, const FailerCache& cache) const {
		if (state.value == 1) {
			return cache.checkRecord(value(), 0, state.source);
		}
		else {
			return cache.checkRecord(state.node, 1, state.source);
		}
	}
}
