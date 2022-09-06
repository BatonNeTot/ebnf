#ifndef node_literal_h
#define node_literal_h

#include "ebnf/node.h"
#include "ebnf/ebnf.h"

namespace ebnf {

	class NodeLiteral : public Node {
	public:
		explicit NodeLiteral(const std::string& value);

		std::string toStr() const override;

		Node* nextChild(const StateInfo&) const override;

		bool updateStr(std::string_view& source) const override;

		const std::string& body() const override;

		std::unique_ptr<Token> token(const std::string_view&) const override;

		bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const override;

		inline const std::string& value() const { return _value; }

	private:
		std::string _value;
	};
}

#endif // !node_literal_h
