#ifndef node_or_h
#define node_or_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeOr : public NodeContainer {
	public:
		std::string toStr() const override;

		Node* nextChild(const StateInfo& state) const override;

		NodeState incrementState(const StateInfo& initialState, const FailerCache& cache) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;
	};
}

#endif // !node_or_h
