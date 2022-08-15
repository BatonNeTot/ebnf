#ifndef parser_h
#define parser_h

#include "exp_lexer.h"
#include "ebnf.h"

#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

namespace ebnf {

	class Parser {
	public:

		Parser(const std::string& source)
			: _source(source) {}

		Ebnf::IdInfo proceedNext();
		bool hasNext() const;

	private:

		class NodeId : public Ebnf::Node {
		public:
			NodeId(const std::string& id)
				: _id(id) {}

			std::string toStr() const override {
				return _id;
			}

			std::string generate(const Ebnf& ebnf) const override {
				auto* node = getById(ebnf, _id);
				return node != nullptr ? node->generate(ebnf) : _id;
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				auto* node = getById(ebnf, _id);
				if (node != nullptr) {
					auto pair = node->tryParse(ebnf, str);
					if (!pair.first) {
						return std::move(pair);
					}
					Ebnf::Token token;
					token.id = _id;
					token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));

					return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
				}

				if (str.length() >= _id.length() && _id == str.substr(0, _id.length())) {
					str = str.substr(_id.length());
					return std::make_pair<bool, Ebnf::Token>(true, { _id, _id });
				}
				else {
					return std::make_pair<bool, Ebnf::Token>(false, {});
				}
			}
		private:
			std::string _id;
		};

		class NodeLiteral : public Ebnf::Node {
		public:
			NodeLiteral(const std::string& value)
				: _value(value) {}

			std::string toStr() const override {
				return '\'' + _value + '\'';
			}

			std::string generate(const Ebnf&) const override {
				return _value;
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				if (str.length() >= _value.length() && _value == str.substr(0, _value.length())) {
					str = str.substr(_value.length());
					return std::make_pair<bool, Ebnf::Token>(true, { _value });
				}
				else {
					return std::make_pair<bool, Ebnf::Token>(false, {});
				}
			}
		private:
			std::string _value;
		};

		class NodeHolder : public Ebnf::Node {
		public:
			virtual ~NodeHolder() = default;
			virtual void insert(Ebnf::Node* node) = 0;
			virtual Ebnf::Node* pop() = 0;
		};

		class NodeContainer : public NodeHolder {
		public:
			virtual ~NodeContainer() = default;

			void insert(Ebnf::Node* node) override {
				add(node);
			}

			void add(Ebnf::Node* node) {
				_children.emplace_back(node);
				setParent(node, this);
			}

			Ebnf::Node* pop() override {
				return popLast();
			}

			Ebnf::Node* popLast() {
				auto* last = _children.back();
				setParent(last, nullptr);
				_children.pop_back();
				return last;
			}

			const std::vector<Node*>& children() const {
				return _children;
			}

		protected:
			std::string toStr(char separator) const {
				if (_children.empty()) {
					return "";
				}

				auto str = _children.front()->toStr();
				for (auto i = 1u; i < _children.size(); ++i) {
					str += separator + _children[i]->toStr();
				}
				return str;
			}
		private:
			std::vector<Ebnf::Node*> _children;
		};

		class NodeConcat : public NodeContainer {
		public:
			std::string toStr() const override {
				return NodeContainer::toStr(' ');
			}

			std::string generate(const Ebnf& ebnf) const override {
				if (children().empty()) {
					// TODO error!
					return "";
				}

				auto str = children().front()->generate(ebnf);
				for (auto i = 1u; i < children().size(); ++i) {
					str += children()[i]->generate(ebnf);
				}
				return str;
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				Ebnf::Token token;

				for (auto& child : children()) {
					auto pair = child->tryParse(ebnf, str);
					if (!pair.first) {
						return pair;
					}
					token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));
				}

				return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
			}
		};

		class NodeOr : public NodeContainer {
		public:
			std::string toStr() const override {
				return NodeContainer::toStr('|');
			}

			std::string generate(const Ebnf& ebnf) const override {
				if (children().empty()) {
					// TODO error!
					return "";
				}

				auto index = rand() % children().size();
				return children()[index]->generate(ebnf);
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				Ebnf::Token token;
				std::string_view testStr;
				std::string_view outputStr = str;
				bool success = false;

				for (auto& child : children()) {
					testStr = str;
					auto pair = child->tryParse(ebnf, testStr);
					if (pair.first) {
						if (!success || testStr.length() < outputStr.length()) {
							success = true;
							outputStr = testStr;
							token = std::move(pair.second);
						}
					}
				}

				str = outputStr;
				return std::make_pair<bool, Ebnf::Token>(std::move(success), std::move(token));
			}
		};

		class NodeSingle : public NodeHolder {
		public:
			virtual ~NodeSingle() = default;

			void insert(Ebnf::Node* node) override {
				value(node);
			}

			void value(Ebnf::Node* node) {
				if (_node) {
					setParent(_node, nullptr);
				}

				_node = node;
				if (_node) {
					setParent(_node, this);
				}
			}

			Ebnf::Node* pop() override {
				auto output = value();
				value(nullptr);
				return output;
			}

			Ebnf::Node* value() const {
				return _node;
			}
		protected:
			std::string toStr(char left, char right) const {
				return left + _node->toStr() + right;
			}
		private:
			Ebnf::Node* _node = nullptr;
		};

		class NodeBracket : public NodeSingle {
		public:
			std::string toStr() const override {
				return NodeSingle::toStr('(', ')');
			}

			std::string generate(const Ebnf& ebnf) const override {
				return value()->generate(ebnf);
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				if (value() == nullptr) {
					return std::make_pair<bool, Ebnf::Token>(true, {});
				}

				return value()->tryParse(ebnf, str);
			}
		};

		class NodeConditional : public NodeSingle {
		public:
			std::string toStr() const override {
				return NodeSingle::toStr('[', ']');
			}

			std::string generate(const Ebnf& ebnf) const override {
				const auto toBeOrNotToBe = rand() % 2;
				return toBeOrNotToBe > 0 ? value()->generate(ebnf) : "";
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				if (value() == nullptr) {
					return std::make_pair<bool, Ebnf::Token>(true, {});
				}

				auto testStr = str;
				auto pair = value()->tryParse(ebnf, testStr);
				if (pair.first) {
					str = testStr;
					return std::move(pair);
				}
				return std::make_pair<bool, Ebnf::Token>(true, {});
			}
		};

		class NodeRepetition : public NodeSingle {
		public:
			std::string toStr() const override {
				return NodeSingle::toStr('{', '}');
			}

			std::string generate(const Ebnf& ebnf) const override {
				std::string str;
				const auto count = rand() % 10;
				for (auto i = 0; i < count; ++i) {
					str += value()->generate(ebnf);
				}
				return str;
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str) const override {
				Ebnf::Token token;

				if (value() == nullptr) {
					return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
				}

				auto testStr = str;
				while (true) {
					auto pair = value()->tryParse(ebnf, testStr);
					if (pair.first) {
						str = testStr;
						token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));
					}
					else {
						break;
					}
				}
				return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
			}
		};

		enum class Bracket : uint8_t {
			Regular,
			Square,
			Curly,
		};

		inline Ebnf::IdInfo&& destroyAndReturn() {
			_lastNode = nullptr;
			_lastHolder = nullptr;
			return std::move(_currentId);
		}

		template <class T, class... Args>
		T* create(Args&&... args) {
			return _currentId.create<T>(std::forward<Args>(args)...);
		}

		void proceedNode(Ebnf::Ebnf::Node* node);

		bool throwError(const std::string& message = "");

		std::string _source;
		uint64_t _carret = 0;

		std::stack<Bracket> _brackets;

		Ebnf::IdInfo _currentId;
		Ebnf::Ebnf::Node* _lastNode = nullptr;
		Ebnf::Ebnf::Node* _lastHolder = nullptr;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !parser_h
