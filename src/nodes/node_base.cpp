#include "node_base.h"

namespace ebnf {

	bool NodeHolder::updateStr(std::string_view& source) const {
		return true;
	}

	std::unique_ptr<Token> NodeHolder::token(const std::string_view&) const {
		return std::make_unique<Token>();
	}


	void NodeContainer::insert(NodeBase* node) {
		add(node);
	}

	void NodeContainer::add(NodeBase* node) {
		_children.emplace_back(node);
		setParent(node, this);
	}

	NodeBase* NodeContainer::pop() {
		return popLast();
	}

	NodeBase* NodeContainer::popLast() {
		auto* last = _children.back();
		setParent(last, nullptr);
		_children.pop_back();
		return last;
	}

	const std::vector<NodeBase*>& NodeContainer::children() const {
		return _children;
	}

	std::string NodeContainer::toStr(char separator) const {
		if (_children.empty()) {
			return "";
		}

		auto str = _children.front()->toStr();
		for (auto i = 1u; i < _children.size(); ++i) {
			str += separator + _children[i]->toStr();
		}
		return str;
	}


	void NodeSingle::insert(NodeBase* node) {
		value(node);
	}

	void NodeSingle::value(NodeBase* node) {
		if (_node) {
			setParent(_node, nullptr);
		}

		_node = node;
		if (_node) {
			setParent(_node, this);
		}
	}

	NodeBase* NodeSingle::pop() {
		auto output = value();
		value(nullptr);
		return output;
	}

	NodeBase* NodeSingle::value() const {
		return _node;
	}
	std::string NodeSingle::toStr(char left, char right) const {
		return left + _node->toStr() + right;
	}
}
