#ifndef ebnf_node_h
#define ebnf_node_h

#include "token.h"

#include <string>
#include <list>
#include <unordered_map>
#include <stack>

namespace ebnf {

	class Ebnf;
	class Node;
	using NodeState = uint64_t;

	struct SourceInfo {
		explicit SourceInfo() = default;
		explicit SourceInfo(const std::string_view& str_)
			: str(str_) {}
		SourceInfo(const std::string_view& str_, uint64_t line_, uint64_t offset_)
			: str(str_), line(line_), offset(offset_) {}

		std::string_view str;
		uint64_t line = 0;
		uint64_t offset = 0;
	};

	struct StateInfo {
		StateInfo(Node* node_, StateInfo* parent_, uint64_t childIndex_)
			: node(node_), parent(parent_), childIndex(childIndex_) {}
		StateInfo(Node* node_, StateInfo* parent_, uint64_t childIndex_, const SourceInfo& source_)
			: node(node_), parent(parent_), childIndex(childIndex_), source(source_) {}

		Node* node = nullptr;
		StateInfo* parent = nullptr;
		uint64_t childIndex = 0;
		NodeState value = 0;
		SourceInfo source;
		Token* token = nullptr;
	};

	class StateStack {
	public:

		bool empty() const;

		void push(Node* node);
		void push(Node* node, StateInfo* parent, uint64_t childIndex);

		void push(Node* node, std::string_view str);
		void push(Node* node, StateInfo* parent, uint64_t childIndex, const SourceInfo& source);

		StateInfo& top();
		const StateInfo& top() const;

		void pop();

		Token buildTokens(const Ebnf& ebnf);

	private:

		std::list<StateInfo> _stack;
	};

	class Node {
	public:
		virtual ~Node() = default;

		Node* parent() const {
			return _parent;
		}

		virtual std::string toStr() const = 0;

		Node* next(const Ebnf& ebnf, StateInfo& state, StateInfo*& parentState) const {
			if (auto* firstChild = nextChild(ebnf, state, nullptr)) {
				parentState = &state;
				return firstChild;
			}

			auto* childState = &state;
			parentState = state.parent;
			Node* nextNode = nullptr;
			while (parentState != nullptr &&
				(nextNode = parentState->node->nextChild(ebnf, *parentState, childState)) == nullptr) {
				childState = parentState;
				parentState = parentState->parent;
			}
			return nextNode;
		}

		virtual Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const = 0;

		virtual NodeState incrementState(NodeState initialState, const Node* failedNode) const = 0;

		virtual bool updateStr(const Ebnf& ebnf, SourceInfo& source) const = 0;

		virtual const std::string& body(const Ebnf&) const { static std::string empty; return empty; }

		virtual std::unique_ptr<Token> token(const Ebnf&) const { return std::make_unique<Token>(); }

		bool isDeepChild(const Node* child) const {
			if (child == nullptr) {
				return false;
			}

			const Node* parent = nullptr;
			while ((parent = child->parent()) != nullptr) {
				if (parent == this) {
					return true;
				}
			}

			return false;
		}
	protected:

		static void setParent(Node* node, Node* parent) {
			node->_parent = parent;
		}

		static Node* getById(const Ebnf& ebnf, const std::string& id);
	private:
		Node* _parent = nullptr;
	};

}

#endif // !ebnf_node_h
