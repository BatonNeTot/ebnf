#include "node_any.h"

namespace ebnf {

	std::string NodeAny::toStr() const {
		return "*";
	}

	Node* NodeAny::nextChild(const StateInfo&) const {
		return nullptr;
	}

	bool NodeAny::updateStr(std::string_view& source) const {
		if (source.length() >= 1) {
			source = source.substr(1);
			return true;
		}
		else {
			return false;
		}
	}

	const std::string& NodeAny::body() const {
		static std::string buffer(1, ' ');
		buffer = static_cast<char>(rand());
		return buffer;
	}

	std::unique_ptr<Token> NodeAny::token(const std::string_view& source) const {
		return std::make_unique<Token>(std::string(1, source[0]));
	}

	bool NodeAny::readyForFailerCache(const StateInfo&, const FailerCache&) const {
		return true;
	}
}
