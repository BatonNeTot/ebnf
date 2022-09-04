#include "node_brackets.h"

namespace ebnf {

	std::string NodeBrackets::toStr() const {
		return NodeSingle::toStr('(', ')');
	}

	Node* NodeBrackets::nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const {
		if (state.value == 0) {
			return nullptr;
		}

		return after == nullptr ? value() : nullptr;
	}
}
