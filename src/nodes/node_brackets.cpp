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

	bool NodeBrackets::readyForFailerCache(const StateInfo&, const FailerCache&) const {
		return true;
	}
}
