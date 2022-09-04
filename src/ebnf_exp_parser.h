#ifndef ebnf_exp_parser_h
#define ebnf_exp_parser_h

#include "ebnf/node.h"
#include "ebnf/ebnf.h"

#include <string>
#include <stack>

namespace ebnf {

	class EbnfExpParser {
	public:

		EbnfExpParser(Ebnf::IdInfo& info)
			: _info(info) {}

		size_t fillInfo(const std::string_view& expression);

	private:

		enum class Bracket : uint8_t {
			Regular,
			Square,
			Curly,
		};

		template <class T, class... Args>
		T* create(Args&&... args) {
			return _info.create<T>(std::forward<Args>(args)...);
		}

		void proceedNode(Node* node);

		bool throwError(const std::string& message = "");

		std::stack<Bracket> _brackets;

		Ebnf::IdInfo& _info;
		Node* _lastNode = nullptr;
		Node* _lastHolder = nullptr;

		bool _errorFlag = false;
		std::string _errorMsg;
	};

}

#endif // !ebnf_exp_parser_h
