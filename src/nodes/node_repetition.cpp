#include "node_repetition.h"

namespace ebnf {

	std::string NodeRepetition::toStr() const {
		return NodeSingle::toStr('{', '}');
	}

	Node* NodeRepetition::nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const {
		if (state.value == 0 || state.value == 1) {
			return nullptr;
		}

		return (after == nullptr || after->childIndex + 2 < state.value) ? value() : nullptr;
	}

	NodeState NodeRepetition::incrementState(NodeState initialState, const Node* failedNode) const {
		return isDeepChild(failedNode) ? 0 : initialState + 1;
	}
}
