#ifndef node_concat_h
#define node_concat_h

#include "node_base.h"

namespace ebnf {

	class NodeConcat : public NodeContainer {
	public:
		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const override;
	};
}

#endif // !node_concat_h
