#ifndef ebnf_h
#define ebnf_h

#include <string>
#include <unordered_map>
#include <stack>
#include <memory>

namespace ebnf {

	class Parser;
	class ExpParser;

	class Ebnf {
	public:

		Ebnf(const std::string& source);

		std::string generateFor(const std::string& id) const;

		struct Token {
			Token() = default;
			Token(const std::string & value_)
				: value(value_) {}
			Token(const std::string & id_, const std::string & value_)
				: id(id_)
				, value(value_) {}
			Token(uint64_t line_, uint64_t offset_)
				: line(line_)
				, offset(offset_) {}

			Token(Token& token) = delete;
			Token(const Token& token) = delete;
			Token(Token&& token)
				: id(std::move(token.id))
				, value(std::move(token.value))
				, line(token.line)
				, offset(token.offset)
				, parts(std::move(token.parts)) {}

			Token& operator= (Token&& token) {
				id = std::move(token.id);
				value = std::move(token.value);
				line = token.line;
				offset = token.offset;
				parts = std::move(token.parts);
				return *this;
			}

			std::string toStr() const {
				if (!value.empty()) {
					return value;
				}

				std::string str;

				for (auto& part : parts) {
					str += part->toStr();
				}

				return str;
			}

			std::string id;
			std::string value;
			uint64_t line = 0;
			uint64_t offset = 0;
			std::vector<std::unique_ptr<Token>> parts;
		};

		std::pair<bool, Token> parseAs(const std::string& str, const std::string& expression) const;

	private:

		friend Parser;
		friend ExpParser;

		class Node {
		public:
			virtual ~Node() = default;

			Node* parent() const {
				return _parent;
			}

			virtual std::string toStr() const = 0;

			virtual void generate(const Ebnf& ebnf, std::string& output, std::stack<Node*>& stack) const = 0;

			virtual size_t generationWeight() const { return 1; }

			virtual std::pair<bool, Token> tryParse(const Ebnf& ebnf, std::string_view& str, uint64_t& line, uint64_t& offset) const = 0;
		protected:
			static void setParent(Node* node, Node* parent) {
				node->_parent = parent;
			}

			static Node* getById(const Ebnf& ebnf, const std::string& id) {
				return ebnf.getById(id);
			}
		private:
			Node* _parent = nullptr;
		};

		struct IdInfo {
			IdInfo() = default;
			IdInfo(IdInfo& other) = delete;
			IdInfo(const IdInfo& other) = delete;
			IdInfo(IdInfo&& other)
				: id(std::move(other.id))
				, tree(other.tree)
				, type(other.type)
				, _nodeHolder(std::move(other._nodeHolder)) {}

			IdInfo& operator =(IdInfo&& other) {
				id = std::move(other.id);
				tree = other.tree;
				type = other.type;
				_nodeHolder = std::move(other._nodeHolder);
				return *this;
			}

			std::string id;
			Node* tree = nullptr;

			enum class Type : uint8_t {
				Default,
				Base,
				Boilerplate
			};
			Type type = Type::Default;

		private:
			friend Parser;
			friend ExpParser;

			template <class T, class... Args>
			T* create(Args&&... args) {
				auto uniquePtr = std::make_unique<T>(std::forward<Args>(args)...);
				auto ptr = uniquePtr.get();
				_nodeHolder.emplace_back(std::move(uniquePtr));
				return ptr;
			}

			std::list<std::unique_ptr<Node>> _nodeHolder;
		};

		IdInfo::Type getIdType(const std::string& id) const {
			auto it = _ids.find(id);
			return it != _ids.end() ? it->second.type : IdInfo::Type::Base;
		}

		Node* getById(const std::string& id) const {
			auto it = _ids.find(id);
			return it != _ids.end() ? it->second.tree : nullptr;
		}

		bool throwError(const std::string& message = "");

		std::unordered_map<std::string, IdInfo> _ids;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !ebnf_h
