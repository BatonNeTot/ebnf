#include "ebnf/node.h"

#include "ebnf/ebnf.h"

namespace ebnf {

	bool StateStack::empty() const {
		return _stack.empty();
	}

	void StateStack::push(Node* node) {
		push(node, nullptr, 0);
	}
	void StateStack::push(Node* node, StateInfo* parent, uint64_t childIndex) {
		_stack.emplace_back(node, parent, childIndex);
	}

	void StateStack::push(Node* node, std::string_view str) {
		push(node, nullptr, 0, SourceInfo(str));
	}
	void StateStack::push(Node* node, StateInfo* parent, uint64_t childIndex, const SourceInfo& source) {
		_stack.emplace_back(node, parent, childIndex, source);
	}

	StateInfo& StateStack::top() {
		return _stack.back();
	}

	const StateInfo& StateStack::top() const {
		return _stack.back();
	}

	void StateStack::pop() {
		_stack.pop_back();
	}

	Token StateStack::buildTokens(const Ebnf& ebnf) {
		Token root;

		for (auto& state : _stack) {
			if (state.parent == nullptr) {
				auto rootPtr = state.node->token(ebnf);
				root = std::move(*rootPtr);
				state.token = &root;
				continue;
			}

			auto& token = state.parent->token->parts
				.emplace_back(state.node->token(ebnf));
			state.token = token.get();
		}

		_stack.clear();

		return root;
	}

	Node* Node::getById(const Ebnf& ebnf, const std::string& id) {
		return ebnf.getById(id);
	}

}