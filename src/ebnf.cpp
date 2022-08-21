#include "ebnf.h"

#include "parser.h"

namespace ebnf {

	Ebnf::Ebnf(const std::string& source) {
		Parser parser(source);

		while (parser.hasNext()) {
			auto info = std::move(parser.proceedNext());
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

	std::pair<bool, Ebnf::Token> Ebnf::parseAs(const std::string& str, const std::string& id) const {
		auto* node = getById(id);
		if (node == nullptr) {
			if (str == id) {
				return std::make_pair<bool, Ebnf::Token>(true, {id, id});
			}
			else {
				return std::make_pair<bool, Ebnf::Token>(false, {});
			}
		}

		auto strView = std::string_view(str);
		auto pair = std::move(node->tryParse(*this, strView));
		if (!pair.first || !strView.empty()) {
			return std::make_pair<bool, Ebnf::Token>(false, {});
		}

		Ebnf::Token token;
		token.id = id;
		token.parts.emplace_back(std::make_unique<Ebnf::Token>(std::move(pair.second)));

		return std::make_pair<bool, Ebnf::Token>(true, std::move(token));
	}

}