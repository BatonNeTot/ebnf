#include "node_conditional.h"

namespace ebnf {

	std::string NodeConditional::toStr() const {
		return NodeSingle::toStr('[', ']');
	}

	Node* NodeConditional::nextChild(const StateInfo& state) const {
		if (state.value == 0 || state.value == 1) {
			return nullptr;
		}

		return state.nextChildIndex == 0 ? value() : nullptr;
	}

	NodeState NodeConditional::incrementState(const StateInfo& initialState, const FailerCache& cache) const {
		auto state = initialState.value + 1;
		return state <= 2 ? state : 0;
	}

	bool NodeConditional::readyForFailerCache(const StateInfo& state, const FailerCache& cache) const {
		switch (state.value) {
		case 0: {
			return cache.checkRecord(state.node, 1, state.source)
				&& cache.checkRecord(state.node, 2, state.source);
		}
		case 1: {
			return true;
		}
		case 2: {
			return cache.checkRecord(value(), 0, state.source);
		}
		}
		return true;
	}
}
