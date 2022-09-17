#include "node_repetition.h"

namespace ebnf {

	std::string NodeRepetition::toStr() const {
		return NodeSingle::toStr('{', '}');
	}

	Node* NodeRepetition::nextChild(const StateInfo& state) const {
		if (state.value == 0 || state.value == 1) {
			return nullptr;
		}

		return state.nextChildIndex + 1 < state.value ? value() : nullptr;
	}

	NodeState NodeRepetition::maxState(const StateInfo& state) const {
		return state.mostSuccessfulChild + 2;
	}

	bool NodeRepetition::readyForFailerCache(const StateInfo& state, const FailerCache& cache) const {
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
		default: {
			return false;
		}
		}
	}
}
