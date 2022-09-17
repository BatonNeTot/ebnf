#ifndef node_repetition_h
#define node_repetition_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeRepetition : public NodeSingle {
	public:
		std::string toStr() const override;

		Node* nextChild(const StateInfo& state) const override;

		NodeState maxState(const StateInfo& state) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;
	};
}

#endif // !node_repetition_h
