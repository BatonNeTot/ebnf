#include "node_id.h"

namespace ebnf {

	NodeId::NodeId(const std::string& id)
		: NodeLiteral(id) {}

	Node* NodeId::node(const Ebnf& ebnf) const {
		if (_node == nullptr) {
			_node = getById(ebnf, value());
		}
		return _node;
	}

	std::string NodeId::toStr() const {
		return value();
	}

	Node* NodeId::nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const {
		auto node = NodeId::node(ebnf);
		if (node == nullptr) {
			return nullptr;
		}

		if (state.value == 0) {
			return nullptr;
		}

		return after == nullptr ? node : nullptr;
	}

	NodeState NodeId::incrementState(NodeState initialState, const Node*) const {
		return initialState == 0 ? 1 : 0;
	}

	bool NodeId::updateStr(const Ebnf& ebnf, SourceInfo& source) const {
		if (node(ebnf) != nullptr) {
			return true;
		}

		return NodeLiteral::updateStr(ebnf, source);
	}

	const std::string& NodeId::body(const Ebnf& ebnf) const {
		return node(ebnf) == nullptr ? value() : Node::body(ebnf);
	}

	std::unique_ptr<Token> NodeId::token(const Ebnf& ebnf, const SourceInfo&) const {
		if (node(ebnf)) {
			auto token = std::make_unique<Token>();
			token->id = value();
			return token;
		}
		else {
			return std::make_unique<Token>(value(), value());
		}
	}
}
