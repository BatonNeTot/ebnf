#ifndef node_or_h
#define node_or_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeOr : public NodeContainer {
	public:
		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const override;

		NodeState incrementState(const StateInfo& initialState, const StateInfo* failedNode) const override;
	};
}

#endif // !node_or_h
