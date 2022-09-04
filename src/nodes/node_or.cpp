#include "node_or.h"

namespace ebnf {

	std::string NodeOr::toStr() const {
		return NodeContainer::toStr('|');
	}

	Node* NodeOr::nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const {
		if (state.value == 0) {
			return nullptr;
		}

		return after == nullptr ? children()[state.value - 1] : nullptr;
	}

	NodeState NodeOr::incrementState(NodeState initialState, const Node*) const {
		auto state = initialState + 1;
		return state <= children().size() ? state : 0;
	}
}
