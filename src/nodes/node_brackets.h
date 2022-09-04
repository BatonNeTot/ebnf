#ifndef node_brackets_h
#define node_brackets_h

#include "node_base.h"

namespace ebnf {

	class NodeBrackets : public NodeSingle {
	public:
		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const override;
	};
}

#endif // !node_brackets_h
