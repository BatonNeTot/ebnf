#include "node_literal.h"

namespace ebnf {

	NodeLiteral::NodeLiteral(const std::string& value)
		: _value(value) {}

	std::string NodeLiteral::toStr() const {
		return '\'' + _value + '\'';
	}

	Node* NodeLiteral::nextChild(const StateInfo&) const {
		return nullptr;
	}

	bool NodeLiteral::updateStr(std::string_view& source) const {
		if (source.length() < _value.length()) {
			return false;
		}

		for (auto i = 0u; i < _value.length(); ++i) {
			if (_value[i] != source[i]) {
				return false;
			}
		}
		source = source.substr(_value.length());
		return true;
	}

	const std::string& NodeLiteral::body() const {
		return _value;
	}

	std::unique_ptr<Token> NodeLiteral::token(const std::string_view&) const {
		return std::make_unique<Token>(_value);
	}

	bool NodeLiteral::readyForFailerCache(const StateInfo& state, const FailerCache&cache) const {
		if (state.value == 1) {
			return true;
		}
		else {
			return cache.checkRecord(state.node, 1, state.source);
		}
	}
}
