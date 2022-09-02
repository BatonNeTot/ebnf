#include "ebnf.h"

#include "parser.h"
#include "exp_parser.h"

namespace ebnf {

	Ebnf::Ebnf(const std::string& source) {
		Parser parser(source);

		while (parser.hasNext()) {
			auto info = parser.proceedNext();
			auto id = info.id;
			_ids.try_emplace(id, std::move(info));
		}
	}

	std::string Ebnf::generateFor(const std::string& id) const {
		auto* root = getById(id);
		if (root == nullptr) {
			return id;
		}

		std::string output;
		std::stack<Node*> stack;

		stack.emplace(root);

		while (!stack.empty()) {
			auto* node = stack.top();
			stack.pop();
			node->generate(*this, output, stack);
		}

		return output;
	}

	std::pair<bool, Ebnf::Token> Ebnf::parseAs(const std::string& str, const std::string& expression) const {
		IdInfo info;
		ExpParser expParser(info);
		expParser.fillInfo(expression);

		auto strView = std::string_view(str);
		return info.tree->tryParse(*this, strView);
	}

}