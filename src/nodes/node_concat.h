#ifndef node_concat_h
#define node_concat_h

#include "node_base.h"

namespace ebnf {

	class NodeConcat : public NodeContainer {
	public:
		std::string toStr() const override;

		Node* nextChild(const StateInfo& state) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;
	};
}

#endif // !node_concat_h
