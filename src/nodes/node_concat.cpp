#include "node_concat.h"

namespace ebnf {

	std::string NodeConcat::toStr() const {
		return NodeContainer::toStr(' ');
	}

	Node* NodeConcat::nextChild(const StateInfo& state) const {
		if (state.value == 0) {
			return nullptr;
		}

		return children().size() > state.nextChildIndex ? children()[state.nextChildIndex] : nullptr;
	}

	bool NodeConcat::readyForFailerCache(const StateInfo& state, const FailerCache& cache) const {
		if (state.value == 1) {
			return cache.checkRecord(children().front(), 0, state.source);
		}
		else {
			return cache.checkRecord(state.node, 1, state.source);
		}
	}
}
