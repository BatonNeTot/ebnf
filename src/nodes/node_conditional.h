#ifndef node_conditional_h
#define node_conditional_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeConditional : public NodeSingle {
	public:
		std::string toStr() const override;

		Node* nextChild(const StateInfo& state) const override;

		NodeState incrementState(const StateInfo& state, const FailerCache& cache) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;
	};
}

#endif // !node_conditional_h
