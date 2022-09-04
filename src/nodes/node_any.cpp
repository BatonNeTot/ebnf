#include "node_any.h"

namespace ebnf {

	std::string NodeAny::toStr() const {
		return "*";
	}

	Node* NodeAny::nextChild(const Ebnf& ebnf, const StateInfo&, const StateInfo*) const {
		return nullptr;
	}

	bool NodeAny::updateStr(const Ebnf& ebnf, SourceInfo& source) const {
		if (source.str().length() >= 1) {
			source.substruct(source.str()[0]);
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
		return std::make_unique<Token>(std::string(1, source.str()[0]));
	}
}
