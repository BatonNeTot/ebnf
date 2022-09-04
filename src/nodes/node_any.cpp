#include "node_any.h"

namespace ebnf {

	std::string NodeAny::toStr() const {
		return "*";
	}

	Node* NodeAny::nextChild(const Ebnf& ebnf, const StateInfo&, const StateInfo*) const {
		return nullptr;
	}

	NodeState NodeAny::incrementState(NodeState initialState, const Node*) const {
		return initialState == 0 ? 1 : 0;
	}

	bool NodeAny::updateStr(const Ebnf& ebnf, SourceInfo& source) const {
		if (source.str.length() >= 1) {
			static std::string lineSeparator = "\n";
			if (lineSeparator.size() == 1 && lineSeparator[0] == source.str[0]) {
				++source.line;
				source.offset = 0;
			}
			else {
				++source.offset;
			}
			source.str = source.str.substr(1);
			return true;
		}
		else {
			return false;
		}
	}

	const std::string& NodeAny::body(const Ebnf&) const {
		static std::string buffer(1, ' ');
		buffer = static_cast<char>(rand());
		return buffer;
	}

	std::unique_ptr<Token> NodeAny::token(const Ebnf&, const SourceInfo& source) const {
		return std::make_unique<Token>(std::string(1, source.str[0]));
	}
}
