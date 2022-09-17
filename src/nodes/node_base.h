#ifndef node_base_h
#define node_base_h

#include "ebnf/node.h"

namespace ebnf {

	class NodeBase : public Node {
	public:

		NodeBase* parent() const {
			return _parent;
		}
	protected:

		static void setParent(NodeBase* node, NodeBase* parent) {
			node->_parent = parent;
		}
	private:
		NodeBase* _parent = nullptr;
	};

	class NodeHolder : public NodeBase {
	public:
		virtual ~NodeHolder() = default;
		virtual void insert(NodeBase* node) = 0;
		virtual Node* pop() = 0;

		bool updateStr(std::string_view& source) const override;

		std::unique_ptr<Token> token(const std::string_view&) const override;
	};

	class NodeContainer : public NodeHolder {
	public:
		virtual ~NodeContainer() = default;

		void insert(NodeBase* node) override;

		void add(NodeBase* node);

		NodeBase* pop() override;

		NodeBase* popLast();

		const std::vector<NodeBase*>& children() const;

	protected:
		std::string toStr(char separator) const;
	private:
		std::vector<NodeBase*> _children;
	};

	class NodeSingle : public NodeHolder {
	public:
		virtual ~NodeSingle() = default;

		void insert(NodeBase* node) override;

		void value(NodeBase* node);

		NodeBase* pop() override;

		NodeBase* value() const;

	protected:
		std::string toStr(char left, char right) const;
	private:
		NodeBase* _node = nullptr;
	};
}

#endif // !node_base_h
