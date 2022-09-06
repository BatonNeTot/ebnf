#ifndef node_brackets_h
#define node_brackets_h

#include "node_base.h"

namespace ebnf {

	class NodeBrackets : public NodeSingle {
	public:
		std::string toStr() const override;

		Node* nextChild(const StateInfo& state) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;
	};
}

#endif // !node_brackets_h
