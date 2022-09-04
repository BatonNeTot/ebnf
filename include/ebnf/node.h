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

	class SourceInfo {
	public:
		static const std::string lineSeparator;

		explicit SourceInfo() = default;
		explicit SourceInfo(const std::string_view& str)
			: _str(str) {}
		SourceInfo(uint64_t line, uint64_t offset)
			: _line(line), _offset(offset) {}
		SourceInfo(const std::string_view& str, uint64_t line, uint64_t offset)
			: _str(str), _line(line), _offset(offset) {}

		inline const std::string_view& str() const {
			return _str;
		}

		inline uint64_t line() const {
			return _line;
		}

		inline uint64_t offset() const {
			return _offset;
		}

		struct CachedDiff {
			explicit CachedDiff(const std::string& str)
				: _length(str.length()) {
				std::string::size_type nextLinePos = 0;

				while ((nextLinePos = str.find(lineSeparator, nextLinePos)) != std::string::npos) {
					++_lines;
					nextLinePos += lineSeparator.length();
					_offset = nextLinePos;
				}
			}

			inline uint64_t length() const {
				return _length;
			}
			inline uint64_t lines() const {
				return _lines;
			}
			inline uint64_t offset() const {
				return _offset;
			}

		private:
			uint64_t _length = 0;
			uint64_t _lines = 0;
			uint64_t _offset = 0;
		};

		void substruct(char symbol) {
			if (lineSeparator.size() == 1 && lineSeparator[0] == symbol) {
				++_line;
				_offset = 0;
			}
			else {
				++_offset;
			}
			_str = _str.substr(1);
		}

		void substruct(const std::string& str) {
			substruct(CachedDiff(str));
		}

		void substruct(const CachedDiff& diff) {
			_str = _str.substr(diff.length());
			_line += diff.lines();
			_offset = diff.lines() > 0 ? _offset : diff.offset() + diff.length();
		}

	private:
		std::string_view _str;
		uint64_t _line = 0;
		uint64_t _offset = 0;
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

	struct Record {
		Record(const Node* node_, const SourceInfo& anchorSource_)
			: node(node_), anchorLine(anchorSource_.line()), anchorOffset(anchorSource_.offset()), success(false) {}
		Record(const Node* node_, const SourceInfo& anchorSource_, const SourceInfo& source_)
			: node(node_), anchorLine(anchorSource_.line()), anchorOffset(anchorSource_.offset()), source(source_), success(true) {}

		friend bool operator==(const Record& lhs, const Record& rhs) {
			return lhs.node == rhs.node && lhs.anchorLine == rhs.anchorLine && lhs.anchorOffset == rhs.anchorOffset;
		}

		const Node* node;
		uint64_t anchorLine;
		uint64_t anchorOffset;
		SourceInfo source;
		bool success = false;
	};
}

template<>
struct std::hash<ebnf::Record>
{
	std::size_t operator()(const ebnf::Record& record) const noexcept {
		std::size_t hNode = std::hash<const ebnf::Node*>{}(record.node);
		std::size_t nLine = std::hash<uint64_t>{}(record.anchorOffset);
		std::size_t hOffset = std::hash<uint64_t>{}(record.anchorOffset);
		return hNode ^ (nLine << 1) ^ (hOffset << 2);
	}
};

namespace ebnf {

	class Cache {
	public:

		inline const Record* checkRecord(Node* node, const SourceInfo& anchorSource) const {
			return checkRecord(Record(node, anchorSource));
		}

		const Record* checkRecord(const Record& record) const {
			auto it = _cache.find(record);
			return it != _cache.end() ? &*it : nullptr;
		}

		inline void recordSuccess(const Node* node, const SourceInfo& anchorSource, const SourceInfo& source) {
			record(Record(node, anchorSource, source));
		}

		inline void recordFailer(const Node* node, const SourceInfo& anchorSource) {
			record(Record(node, anchorSource));
		}

		void record(Record&& record) {
			auto [it, success] = _cache.emplace(record);
			if (!success) {
				_cache.erase(it);
				_cache.emplace(record);
			}
		}

	private:
		std::unordered_set<Record> _cache;
	};

	class StateStack {
	public:

		bool empty() const;

		void push(Node* node);
		void push(Node* node, StateInfo* parent, uint64_t childIndex);

		void push(Node* node, std::string_view str);
		void push(Node* node, StateInfo* parent, uint64_t childIndex, SourceInfo source);

		StateInfo& top();
		const StateInfo& top() const;

		void pop();

		Token buildTokens(const Ebnf& ebnf);

	private:

		void flush();

		std::deque<StateInfo> _stack;
		decltype(_stack.begin()) _top;
	};

	class Node {
	public:
		virtual ~Node() = default;

		Node* parent() const {
			return _parent;
		}

		virtual std::string toStr() const = 0;

		Node* siblingOrNext(const Ebnf& ebnf, StateInfo& state, StateInfo*& parentState) const {
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

		Node* next(const Ebnf& ebnf, StateInfo& state, StateInfo*& parentState) const {
			if (auto* firstChild = nextChild(ebnf, state, nullptr)) {
				parentState = &state;
				return firstChild;
			}

			return siblingOrNext(ebnf, state, parentState);
		}

		virtual Node* nextChild(const Ebnf& ebnf, const StateInfo& state, const StateInfo* after) const = 0;

		virtual NodeState incrementState(const StateInfo& initialState, const StateInfo*) const {
			return initialState.value == 0 ? 1 : 0;
		}

		virtual bool updateStr(const Ebnf& ebnf, SourceInfo& source) const = 0;

		bool isCacheable() const { return parent() == nullptr; }

		virtual const std::string& body(const Ebnf&) const { static std::string empty; return empty; }

		virtual std::unique_ptr<Token> token(const Ebnf&, const SourceInfo&) const = 0;

		static bool isDeepChild(const StateInfo& node, const StateInfo* child) {
			if (child == nullptr) {
				return false;
			}

			const StateInfo* parent = nullptr;
			while ((parent = child->parent) != nullptr) {
				if (parent == &node) {
					return true;
				}
				child = parent;
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
