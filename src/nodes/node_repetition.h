#ifndef node_repetition_h
#define node_repetition_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeRepetition : public NodeSingle {
	public:
		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const override;

		NodeState incrementState(NodeState state, const Node* failedNode) const override;
	};
}

#endif // !node_repetition_h
