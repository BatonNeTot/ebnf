#ifndef node_conditional_h
#define node_conditional_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeConditional : public NodeSingle {
	public:
		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const override;

		NodeState incrementState(const StateInfo& state, const StateInfo* failedNode) const override;
	};
}

#endif // !node_conditional_h
