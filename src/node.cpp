#include "ebnf/node.h"

#include "ebnf/ebnf.h"

namespace ebnf {

	const std::string SourceInfo::lineSeparator = "\n";

	bool StateStack::empty() const {
		return _top == _stack.end();
	}

	void StateStack::push(Node* node) {
		push(node, nullptr, 0);
	}
	void StateStack::push(Node* node, StateInfo* parent, uint64_t childIndex) {
		flush();
		_stack.emplace_back(node, parent, childIndex);
		_top = --_stack.end();
	}

	void StateStack::push(Node* node, std::string_view str) {
		push(node, nullptr, 0, SourceInfo(str));
	}
	void StateStack::push(Node* node, StateInfo* parent, uint64_t childIndex, SourceInfo source) {
		flush();
		_stack.emplace_back(node, parent, childIndex, source);
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

	void StateStack::flush() {
		while (!_stack.empty() && (--_stack.end()) != _top) {
			_stack.pop_back();
		}
	}

	Token StateStack::buildTokens(const Ebnf& ebnf) {
		Token root;

		_stack.pop_back();
		for (auto& state : _stack) {
			if (state.parent == nullptr) {
				auto rootPtr = state.node->token(ebnf, state.source);
				root = std::move(*rootPtr);
				state.token = &root;
				continue;
			}

			auto& token = state.parent->token->parts
				.emplace_back(state.node->token(ebnf, state.source));
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