#ifndef node_any_h
#define node_any_h

#include "nodes/node_base.h"

namespace ebnf {

	class NodeAny : public Node {
	public:
		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo&, const StateInfo*) const override;

		bool updateStr(const Ebnf& ebnf, SourceInfo& source) const override;

		const std::string& body(const Ebnf& ebnf) const override;

		std::unique_ptr<Token> token(const Ebnf& ebnf, const SourceInfo& source) const override;
	};
}

#endif // !node_any_h
