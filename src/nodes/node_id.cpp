#include "node_id.h"

namespace ebnf {

	NodeId::NodeId(const std::string& id)
		: NodeLiteral(id) {}

	Node* NodeId::node() const {
		return _node;
	}

	void NodeId::fetch(const Ebnf& ebnf) {
		_node = getById(ebnf, value());
	}

	std::string NodeId::toStr() const {
		return value();
	}

	Node* NodeId::nextChild(const StateInfo& state) const {
		if (node() == nullptr) {
			return nullptr;
		}

		if (state.value == 0) {
			return nullptr;
		}

		return state.nextChildIndex == 0 ? node() : nullptr;
	}

	bool NodeId::updateStr(std::string_view& source) const {
		if (node() != nullptr) {
			return true;
		}

		return NodeLiteral::updateStr(source);
	}

	const std::string& NodeId::body() const {
		return node() == nullptr ? value() : Node::body();
	}

	std::unique_ptr<Token> NodeId::token(const std::string_view&) const {
		if (node()) {
			auto token = std::make_unique<Token>();
			token->id = value();
			return token;
		}
		else {
			return std::make_unique<Token>(value(), value());
		}
	}

	bool NodeId::readyForFailerCache(const StateInfo& state, const FailerCache& cache) const {
		if (state.value == 1) {
			return _node == nullptr ? true : cache.checkRecord(_node, 0, state.source);
		}
		else {
			return cache.checkRecord(state.node, 1, state.source);
		}
	}
}
