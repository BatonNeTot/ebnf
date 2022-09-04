#ifndef node_id_h
#define node_id_h

#include "node_literal.h"

namespace ebnf {

	class NodeId : public NodeLiteral {
	public:
		explicit NodeId(const std::string& id);

		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const override;

		NodeState incrementState(NodeState initialState, const Node* failedNode) const override;

		bool updateStr(const Ebnf& ebnf, SourceInfo& source) const override;

		const std::string& body(const Ebnf&) const override;

		std::unique_ptr<Token> token(const Ebnf& ebnf, const SourceInfo&) const override;

	private:

		Node* node(const Ebnf& ebnf) const;

		mutable Node* _node = nullptr;
	};
}

#endif // !node_id_h
