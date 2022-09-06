#include "ebnf/node.h"

#include "ebnf/ebnf.h"

namespace ebnf {

	bool StateStack::empty() const {
		return _top == _stack.end();
	}

	void StateStack::push(Node* node) {
		push(node, nullptr);
	}
	void StateStack::push(Node* node, StateInfo* parent) {
		push(node, parent, {});
	}

	void StateStack::push(Node* node, std::string_view str) {
		push(node, nullptr, str);
	}
	void StateStack::push(Node* node, StateInfo* parent, std::string_view source) {
		auto end = _stack.end();
		if (_top != end) {
			++_top;
			if (_top != end) {
				*_top = StateInfo(node, parent, std::move(source));
				return;
			}
		}
		_stack.emplace_back(node, parent, std::move(source));
		_top = --_stack.end();
	}

	StateInfo& StateStack::top() {
		return *_top;
	}

	const StateInfo& StateStack::top() const {
		return *_top;
	}

	void StateStack::pop() {
		if (_top == _stack.begin()) {
			_top = _stack.end();
		}
		else {
			--_top;
		}
	}

	Token StateStack::buildTokens(const Ebnf& ebnf) {
		Token root;

		for (auto it = _stack.begin(); it != _top; ++it) {
			auto& state = *it;
			if (state.parent == nullptr) {
				auto rootPtr = state.node->token(state.source);
				root = std::move(*rootPtr);
				state.token = &root;
				continue;
			}

			auto& token = state.parent->token->parts
				.emplace_back(state.node->token(state.source));
			state.token = token.get();
		}

		_stack.clear();
		_top = _stack.end();

		return root;
	}

	Node* Node::getById(const Ebnf& ebnf, const std::string& id) {
		return ebnf.getById(id);
	}

}