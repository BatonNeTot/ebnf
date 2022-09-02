#ifndef exp_parser_h
#define exp_parser_h

#include "ebnf.h"

#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

namespace ebnf {

	class ExpParser {
	public:

		ExpParser(Ebnf::IdInfo& info)
			: _info(info) {}

		size_t fillInfo(const std::string_view& expression);

	private:

		class NodeId : public Ebnf::Node {
		public:
			NodeId(const std::string& id)
				: _id(id) {
				std::string::size_type nextLinePos = 0;

				std::string lineSeparator = "\n";
				while ((nextLinePos = _id.find(lineSeparator, nextLinePos)) != std::string::npos) {
					++_lineIncs;
					nextLinePos += lineSeparator.length();
					_offset = nextLinePos;
				}
			}

			std::string toStr() const override {
				return _id;
			}

			void generate(const Ebnf& ebnf, std::string& output, std::stack<Node*>& stack) const override {
				auto* node = getById(ebnf, _id);
				if (node == nullptr) {
					output += _id;
					return;
				}

				stack.emplace(node);
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				auto* node = getById(ebnf, _id);
				if (node != nullptr) {
					auto pair = node->tryParse(ebnf, str, line, offset);
					if (!pair.first) {
						return pair;
					}

					auto type = ebnf.getIdType(_id);
					switch (type) {
					case Ebnf::IdInfo::Type::Default: {
						if (!pair.second.id.empty()) {
							Ebnf::Token token;
							token.id = _id;
							token.line = pair.second.line;
							token.offset = pair.second.offset;
							token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));

							return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
						}
						else {
							pair.second.id = _id;

							return pair;
						}
					}
					case Ebnf::IdInfo::Type::Base: {
						pair.second.id = _id;
						pair.second.value = pair.second.toStr();
						pair.second.parts.clear();

						return pair;
					}
					case Ebnf::IdInfo::Type::Boilerplate: {
						return std::make_pair<bool, Ebnf::Token>(true, { pair.second.line, pair.second.offset });
					}
					default: {
						return std::make_pair<bool, Ebnf::Token>(false, { line, offset });
					}
					}
				}

				if (str.length() >= _id.length() && _id == str.substr(0, _id.length())) {
					str = str.substr(_id.length());
					return std::make_pair<bool, Ebnf::Token>(true, { _id, _id });
				}
				else {
					return std::make_pair<bool, Ebnf::Token>(false, { line, offset });
				}
				std::pair<bool, Ebnf::Token> pair;
				if (str.length() >= _id.length() && _id == str.substr(0, _id.length())) {
					str = str.substr(_id.length());
					pair = std::make_pair<bool, Ebnf::Token>(true, { _id, _id });

					pair.second.line = line;
					pair.second.offset = offset;

					line += _lineIncs;
					if (_lineIncs > 0) {
						offset = _offset;
					}
					else {
						offset += _id.length();
					}
				}
				else {
					pair = std::make_pair<bool, Ebnf::Token>(false, { line, offset });
				}

				return pair;
			}
		private:
			std::string _id;
			uint64_t _lineIncs = 0;
			uint64_t _offset = 0;
		};

		class NodeLiteral : public Ebnf::Node {
		public:
			NodeLiteral(const std::string& value)
				: _value(value) {
				std::string::size_type nextLinePos = 0;

				std::string lineSeparator = "\n";
				while ((nextLinePos = _value.find(lineSeparator, nextLinePos)) != std::string::npos) {
					++_lineIncs;
					nextLinePos += lineSeparator.length();
					_offset = nextLinePos;
				}
			}

			std::string toStr() const override {
				return '\'' + _value + '\'';
			}

			void generate(const Ebnf&, std::string& output, std::stack<Node*>&) const override {
				output += _value;
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				std::pair<bool, Ebnf::Token> pair;
				if (str.length() >= _value.length() && _value == str.substr(0, _value.length())) {
					str = str.substr(_value.length());
					pair = std::make_pair<bool, Ebnf::Token>(true, { _value });

					pair.second.line = line;
					pair.second.offset = offset;

					line += _lineIncs;
					if (_lineIncs > 0) {
						offset = _offset;
					}
					else {
						offset += _value.length();
					}
				}
				else {
					pair = std::make_pair<bool, Ebnf::Token>(false, { line, offset });
				}

				return pair;
			}
		private:
			std::string _value;
			uint64_t _lineIncs = 0;
			uint64_t _offset = 0;
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

			void generate(const Ebnf&, std::string&, std::stack<Node*>& stack) const override {
				if (children().empty()) {
					// TODO error!
					return;
				}

				for (size_t i = children().size() - 1; i != std::numeric_limits<size_t>::max(); --i) {
					stack.emplace(children()[i]);
				}
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				Ebnf::Token token(line, offset);

				for (auto& child : children()) {
					auto pair = child->tryParse(ebnf, str, line, offset);
					if (!pair.first) {
						return pair;
					}
					if (!pair.second.value.empty() || !pair.second.parts.empty()) {
						token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));
					}
				}

				if (!token.parts.empty()) {
					token.line = token.parts.front()->line;
					token.offset = token.parts.front()->offset;
				}
				return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
			}
		};

		class NodeOr : public NodeContainer {
		public:
			std::string toStr() const override {
				return NodeContainer::toStr('|');
			}

			void generate(const Ebnf&, std::string&, std::stack<Node*>& stack) const override {
				if (children().empty()) {
					// TODO error!
					return;
				}

				size_t randValue = static_cast<size_t>(rand()) % generationWeight();
				size_t totalWeight = 0;
				
				for (auto& child : children()) {
					if (randValue <= totalWeight) {
						stack.emplace(child);
						return;
					}

					totalWeight += child->generationWeight();
				}
			}

			size_t generationWeight() const override {
				size_t totalWeight = 0;
				for (auto& child : children()) {
					totalWeight += child->generationWeight();
				}
				return totalWeight;
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				Ebnf::Token token(line, offset);
				std::string_view testStr;
				std::string_view outputStr = str;
				uint64_t tempLine = line;
				auto outputLine = line;
				uint64_t tempOffset = offset;
				auto outputOffset = offset;
				bool success = false;

				for (auto& child : children()) {
					testStr = str;
					tempLine = line;
					tempOffset = offset;
					auto pair = child->tryParse(ebnf, testStr, tempLine, tempOffset);
					if (pair.first) {
						success = true;
					}
					if ((!success || pair.first) && testStr.length() < outputStr.length()) {
						if (pair.first) {
							outputStr = testStr;
							outputLine = tempLine;
							outputOffset = tempOffset;
						}
						token = std::move(pair.second);
					}
				}

				str = outputStr;
				line = outputLine;
				offset = outputOffset;
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

			void generate(const Ebnf&, std::string&, std::stack<Node*>& stack) const override {
				if (value()) {
					stack.emplace(value());
				}
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				if (value() == nullptr) {
					return std::make_pair<bool, Ebnf::Token>(true, { line, offset });
				}

				return value()->tryParse(ebnf, str, line, offset);
			}
		};

		class NodeConditional : public NodeSingle {
		public:
			std::string toStr() const override {
				return NodeSingle::toStr('[', ']');
			}

			void generate(const Ebnf&, std::string&, std::stack<Node*>& stack) const override {
				const auto toBeOrNotToBe = rand() % 2;
				if (toBeOrNotToBe) {
					stack.emplace(value());
				}
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				if (value() == nullptr) {
					return std::make_pair<bool, Ebnf::Token>(true, { line, offset });
				}

				auto testStr = str;
				auto pair = value()->tryParse(ebnf, testStr, line, offset);
				if (pair.first) {
					str = testStr;
					return pair;
				}
				return std::make_pair<bool, Ebnf::Token>(true, { line, offset });
			}
		};

		class NodeRepetition : public NodeSingle {
		public:
			std::string toStr() const override {
				return NodeSingle::toStr('{', '}');
			}

			void generate(const Ebnf&, std::string&, std::stack<Node*>& stack) const override {
				const auto count = rand() % 10;
				for (auto i = 0; i < count; ++i) {
					stack.emplace(value());
				}
			}

			std::pair<bool, Ebnf::Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const override {
				Ebnf::Token token(line, offset);

				if (value() == nullptr) {
					return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
				}

				auto testStr = str;
				while (true) {
					auto pair = value()->tryParse(ebnf, testStr, line, offset);
					if (pair.first) {
						str = testStr;
						token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));
					}
					else {
						break;
					}
				}

				if (!token.parts.empty()) {
					token.line = token.parts.front()->line;
					token.offset = token.parts.front()->offset;
				}
				return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
			}
		};

		enum class Bracket : uint8_t {
			Regular,
			Square,
			Curly,
		};

		template <class T, class... Args>
		T* create(Args&&... args) {
			return _info.create<T>(std::forward<Args>(args)...);
		}

		void proceedNode(Ebnf::Ebnf::Node* node);

		bool throwError(const std::string& message = "");

		std::stack<Bracket> _brackets;

		Ebnf::IdInfo& _info;
		Ebnf::Ebnf::Node* _lastNode = nullptr;
		Ebnf::Ebnf::Node* _lastHolder = nullptr;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !exp_parser_h