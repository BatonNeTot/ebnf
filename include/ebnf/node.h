#ifndef ebnf_node_h
#define ebnf_node_h

#include "token.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace ebnf {

	class Ebnf;
	class Node;
	using NodeState = uint64_t;

	struct StateInfo {
		StateInfo(Node* node_, StateInfo* parent_)
			: node(node_), parent(parent_) {}
		StateInfo(Node* node_, StateInfo* parent_, const std::string_view& source_)
			: node(node_), parent(parent_), source(source_) {}

		Node* node = nullptr;
		StateInfo* parent = nullptr;
		uint64_t nextChildIndex = 0;
		uint64_t mostSuccessfulChild = 0;
		NodeState value = 0;
		std::string_view source;
		Token* token = nullptr;
	};

	class FailerCache {
	public:

		bool checkRecord(const Node* node, NodeState state, const std::string_view& anchorSource) const {
			auto itLength = _cache.find(anchorSource.length());
			if (itLength == _cache.end()) {
				return false;
			}
			auto itNode = itLength->second.find(node);
			return itNode != itLength->second.end() ? (itNode->second.find(state) != itNode->second.end()) : false;
		}

		void record(const Node* node, NodeState state, const std::string_view& anchorSource) {
			auto [itNodes, lengthSuccess] = _cache.try_emplace(anchorSource.length());
			auto [itState, nodesSuccess] = itNodes->second.try_emplace(node);
			itState->second.emplace(state);
		}

	private:
		std::unordered_map<uint64_t, 
			std::unordered_map<const Node*, 
			std::unordered_set<NodeState>>> _cache;
	};

	class StateStack {
	public:

		bool empty() const;

		void push(Node* node);
		void push(Node* node, StateInfo* parent);

		void push(Node* node, std::string_view str);
		void push(Node* node, StateInfo* parent, std::string_view source);

		StateInfo& top();
		const StateInfo& top() const;

		void pop();

		Token buildTokens(const Ebnf& ebnf);

	private:

		std::list<StateInfo> _stack;
		decltype(_stack.begin()) _top = _stack.end();
	};

	class Node {
	public:
		virtual ~Node() = default;

		Node* parent() const {
			return _parent;
		}

		virtual std::string toStr() const = 0;

		Node* siblingOrNext(StateInfo& state, StateInfo*& parentState) const {
			auto* childState = &state;
			parentState = state.parent;
			Node* nextNode = nullptr;
			while (parentState != nullptr &&
				(nextNode = parentState->node->nextChild(*parentState)) == nullptr) {
				childState = parentState;
				parentState = parentState->parent;
			}
			return nextNode;
		}

		Node* next(StateInfo& state, StateInfo*& parentState) const {
			if (auto* firstChild = nextChild(state)) {
				parentState = &state;
				return firstChild;
			}

			return siblingOrNext(state, parentState);
		}

		virtual Node* nextChild(const StateInfo& state) const = 0;

		virtual NodeState incrementState(const StateInfo& initialState, const FailerCache&) const {
			return initialState.value == 0 ? 1 : 0;
		}

		virtual bool updateStr(std::string_view& source) const = 0;

		virtual const std::string& body() const { static std::string empty; return empty; }

		virtual std::unique_ptr<Token> token(const std::string_view&) const = 0;

		virtual bool readyForFailerCache(const StateInfo& state, const FailerCache& cache) const = 0;

		virtual void fetch(const Ebnf&) {}

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
