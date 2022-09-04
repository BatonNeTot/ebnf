#include "node_base.h"

namespace ebnf {

	bool NodeHolder::updateStr(const Ebnf& ebnf, SourceInfo& source) const {
		return true;
	}


	void NodeContainer::insert(Node* node) {
		add(node);
	}

	void NodeContainer::add(Node* node) {
		_children.emplace_back(node);
		setParent(node, this);
	}

	Node* NodeContainer::pop() {
		return popLast();
	}

	Node* NodeContainer::popLast() {
		auto* last = _children.back();
		setParent(last, nullptr);
		_children.pop_back();
		return last;
	}

	const std::vector<Node*>& NodeContainer::children() const {
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


	void NodeSingle::insert(Node* node) {
		value(node);
	}

	void NodeSingle::value(Node* node) {
		if (_node) {
			setParent(_node, nullptr);
		}

		_node = node;
		if (_node) {
			setParent(_node, this);
		}
	}

	Node* NodeSingle::pop() {
		auto output = value();
		value(nullptr);
		return output;
	}

	Node* NodeSingle::value() const {
		return _node;
	}
	std::string NodeSingle::toStr(char left, char right) const {
		return left + _node->toStr() + right;
	}
}
