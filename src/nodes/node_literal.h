#ifndef node_literal_h
#define node_literal_h

#include "ebnf/node.h"
#include "ebnf/ebnf.h"

namespace ebnf {

	class NodeLiteral : public Node {
	public:
		explicit NodeLiteral(const std::string& value);

		std::string toStr() const override;

		Node* nextChild(const Ebnf& ebnf, const StateInfo&, const StateInfo*) const override;

		NodeState incrementState(NodeState initialState, const Node*) const override;

		bool updateStr(const Ebnf& ebnf, SourceInfo& source) const override;

		const std::string& body(const Ebnf& ebnf) const override;

		std::unique_ptr<Token> token(const Ebnf& ebnf) const override;

		inline const std::string& value() const { return _value; }

	private:
		std::string _value;
		uint64_t _lineIncs = 0;
		uint64_t _offset = 0;
	};
}

#endif // !node_literal_h
