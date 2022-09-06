#ifndef ebnf_ebnf_h
#define ebnf_ebnf_h

#include "token.h"
#include "node.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace ebnf {

	class EbnfParser;
	class EbnfExpParser;

	class Parser;

	class Node;

	enum class ParsingType {
		Incremental,
		Greedy
	};

	class Ebnf {
	public:

		Ebnf(const std::string& source);

		std::string generateFor(const std::string& expression, float incrementChance) const;

		std::pair<bool, Token> parseAs(const std::string& str, const std::string& expression, ParsingType type = ParsingType::Incremental) const;

		Node* getById(const std::string& id) const {
			auto it = _ids.find(id);
			return it != _ids.end() ? it->second.tree : nullptr;
		}

		enum class IdType : uint8_t {
			Default,
			Base,
			Boilerplate
		};

		IdType getIdType(const std::string& id) const {
			auto it = _ids.find(id);
			return it != _ids.end() ? it->second.type : IdType::Default;
		}

	private:

		friend EbnfParser;
		friend EbnfExpParser;

		friend Parser;

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

			void fetch(const Ebnf& ebnf) {
				for (auto& node : _nodeHolder) {
					node->fetch(ebnf);
				}
			}

			IdType type = IdType::Default;
			std::string id;
			Node* tree = nullptr;

		private:
			friend EbnfParser;
			friend EbnfExpParser;

			template <class T, class... Args>
			T* create(Args&&... args) {
				auto uniquePtr = std::make_unique<T>(std::forward<Args>(args)...);
				auto ptr = uniquePtr.get();
				_nodeHolder.emplace_back(std::move(uniquePtr));
				return ptr;
			}

			std::list<std::unique_ptr<Node>> _nodeHolder;
		};

		bool throwError(const std::string& message = "");

		std::unordered_map<std::string, IdInfo> _ids;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !ebnf_ebnf_h
