#include "node_conditional.h"

namespace ebnf {

	std::string NodeConditional::toStr() const {
		return NodeSingle::toStr('[', ']');
	}

	Node* NodeConditional::nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const {
		if (state.value == 0 || state.value == 1) {
			return nullptr;
		}

		return after == nullptr ? value() : nullptr;
	}

	NodeState NodeConditional::incrementState(const StateInfo& initialState, const StateInfo*) const {
		auto state = initialState.value + 1;
		return state <= 2 ? state : 0;
	}
}
