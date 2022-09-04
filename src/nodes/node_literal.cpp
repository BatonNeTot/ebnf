#include "node_literal.h"

namespace ebnf {

	NodeLiteral::NodeLiteral(const std::string& value)
		: _value(value), _diff(value) {}

	std::string NodeLiteral::toStr() const {
		return '\'' + _value + '\'';
	}

	Node* NodeLiteral::nextChild(const Ebnf& ebnf, const StateInfo&, const StateInfo*) const {
		return nullptr;
	}

	bool NodeLiteral::updateStr(const Ebnf& ebnf, SourceInfo& source) const {
		if (source.str().length() >= _value.length() && _value == source.str().substr(0, _value.length())) {
			source.substruct(_diff);
			return true;
		}
		else {
			return false;
		}
	}

	const std::string& NodeLiteral::body(const Ebnf&) const {
		return _value;
	}

	std::unique_ptr<Token> NodeLiteral::token(const Ebnf&, const SourceInfo&) const {
		return std::make_unique<Token>(_value);
	}
}
