#include "node_literal.h"

namespace ebnf {

	NodeLiteral::NodeLiteral(const std::string& value)
		: _value(value) {
		std::string::size_type nextLinePos = 0;

		static std::string lineSeparator = "\n";
		while ((nextLinePos = _value.find(lineSeparator, nextLinePos)) != std::string::npos) {
			++_lineIncs;
			nextLinePos += lineSeparator.length();
			_offset = nextLinePos;
		}
	}

	std::string NodeLiteral::toStr() const {
		return '\'' + _value + '\'';
	}

	Node* NodeLiteral::nextChild(const Ebnf& ebnf, const StateInfo&, const StateInfo*) const {
		return nullptr;
	}

	NodeState NodeLiteral::incrementState(NodeState initialState, const Node*) const {
		return initialState == 0 ? 1 : 0;
	}

	bool NodeLiteral::updateStr(const Ebnf& ebnf, SourceInfo& source) const {
		if (source.str.length() >= _value.length() && _value == source.str.substr(0, _value.length())) {
			source.str = source.str.substr(_value.length());
			source.line += _lineIncs;
			source.offset = _lineIncs > 0 ? _offset : source.offset + _value.length();
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
