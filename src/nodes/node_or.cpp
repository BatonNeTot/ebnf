#include "node_or.h"

namespace ebnf {

	std::string NodeOr::toStr() const {
		return NodeContainer::toStr('|');
	}

	Node* NodeOr::nextChild(const StateInfo& state) const {
		if (state.value == 0) {
			return nullptr;
		}

		return state.nextChildIndex == 0 ? children()[state.value - 1] : nullptr;
	}

	NodeState NodeOr::maxState(const StateInfo&) const {
		return children().size();
	}

	bool NodeOr::readyForFailerCache(const StateInfo& state, const FailerCache& cache) const {
		if (state.value == 0) {
			for (auto i = 1u; i <= children().size(); ++i) {
				if (!cache.checkRecord(state.node, i, state.source)) {
					return false;
				}
			}
			return true;
		}
		else {
			return cache.checkRecord(children()[state.value - 1], 0, state.source);
		}
	}
}
