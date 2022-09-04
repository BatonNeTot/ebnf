#include "node_concat.h"

namespace ebnf {

	std::string NodeConcat::toStr() const {
		return NodeContainer::toStr(' ');
	}

	Node* NodeConcat::nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const {
		if (state.value == 0 || children().empty()) {
			return nullptr;
		}

		if (after == nullptr) {
			return children().front();
		}

		for (auto it = children().cbegin(), end = children().cend(); it != end; ++it) {
			if (*it == after->node) {
				++it;
				return it != end ? *it : nullptr;
			}
		}

		return nullptr;
	}

	NodeState NodeConcat::incrementState(NodeState initialState, const Node*) const {
		return initialState == 0 ? 1 : 0;
	}
}
