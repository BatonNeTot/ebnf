#ifndef node_base_h
#define node_base_h

#include "ebnf/node.h"

namespace ebnf {

	class NodeHolder : public Node {
	public:
		virtual ~NodeHolder() = default;
		virtual void insert(Node* node) = 0;
		virtual Node* pop() = 0;

		bool updateStr(const Ebnf& ebnf, SourceInfo& source) const override;
	};

	class NodeContainer : public NodeHolder {
	public:
		virtual ~NodeContainer() = default;

		void insert(Node* node) override;

		void add(Node* node);

		Node* pop() override;

		Node* popLast();

		const std::vector<Node*>& children() const;

	protected:
		std::string toStr(char separator) const;
	private:
		std::vector<Node*> _children;
	};

	class NodeSingle : public NodeHolder {
	public:
		virtual ~NodeSingle() = default;

		void insert(Node* node) override;

		void value(Node* node);

		Node* pop() override; 

		Node* value() const;

	protected:
		std::string toStr(char left, char right) const;
	private:
		Node* _node = nullptr;
	};
}

#endif // !node_base_h
